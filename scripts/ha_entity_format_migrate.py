#!/usr/bin/env python3
#
# Keeps Home Assistant entity IDs (and therefore history, statistics, dashboards and automations)
# when changing the EMS-ESP "Entity ID format" MQTT setting.
#
# Changing the format changes the uniq_id of EMS-ESP's MQTT Discovery entities, so HA creates new
# entities with new entity IDs and an empty history. This script matches each new entity to the
# old one using the parts of the discovery config that don't depend on the format (state topic,
# command topic and value path), removes the old entity and renames the new one to the old entity ID.
#
# Requires Python 3.9+ and then create a virtual environment:
#  cd scripts
#  python3 -m venv venv
#  source ./venv/bin/activate
#  pip install -r requirements.txt
#
# Create a long-lived access token in HA (Profile -> Security) and take a backup of HA first.
#
# 1. Before changing the format, save the current entities:
#      python3 ha_entity_format_migrate.py --ha-url http://homeassistant.local:8123 --ha-token <token> \
#        --mqtt-host homeassistant.local --mqtt-user <user> --mqtt-password <password> snapshot
# 2. Change the Entity ID format in EMS-ESP (Settings -> MQTT) and wait a minute for it to republish.
# 3. Check what will be changed:
#      python3 ha_entity_format_migrate.py <same options> apply --dry-run
# 4. Apply:
#      python3 ha_entity_format_migrate.py <same options> apply
#
# The token and passwords can also be set with the HA_TOKEN and MQTT_PASSWORD environment variables.

import argparse
import json
import os
import re
import sys
import time

import paho.mqtt.client as mqtt
from termcolor import cprint
from websockets.sync.client import connect

FINGERPRINT_TOPICS = ('stat_t', 'cmd_t', 'temp_cmd_t', 'mode_cmd_t')
FINGERPRINT_TEMPLATES = ('val_tpl', 'temp_stat_tpl')
VALUE_PATH = re.compile(r"value_json(?:\.\w+|\['[^']*'\])+")


def print_success(x):
    return cprint(x, 'green')


def print_warn(x):
    return cprint(x, 'yellow')


def print_fail(x):
    return cprint(x, 'red')


def fingerprint(component, config):
    parts = [component]
    for key in FINGERPRINT_TOPICS:
        parts.append(f"{key}={config.get(key, '')}")
    for key in FINGERPRINT_TEMPLATES:
        m = VALUE_PATH.search(config.get(key, ''))
        parts.append(f"{key}={m.group(0) if m else ''}")
    return '|'.join(parts)


def read_discovery_configs(args):
    """Return all retained EMS-ESP discovery configs as a list of dicts."""
    configs = {}

    def on_message(client, userdata, msg):
        if not msg.payload:
            return
        try:
            config = json.loads(msg.payload)
        except ValueError:
            return
        if not isinstance(config, dict) or config.get('~') != args.base or 'uniq_id' not in config:
            return
        component = msg.topic.split('/')[-4] if msg.topic.count('/') >= 3 else ''
        configs[msg.topic] = {
            'topic': msg.topic,
            'component': component,
            'uniq_id': config['uniq_id'],
            'fingerprint': fingerprint(component, config),
        }

    client = mqtt_connect(args)
    client.on_message = on_message
    client.subscribe(f'{args.discovery_prefix}/#')
    client.loop_start()
    time.sleep(args.wait)
    client.loop_stop()
    client.disconnect()

    return list(configs.values())


def mqtt_connect(args):
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    if args.mqtt_user:
        client.username_pw_set(args.mqtt_user, args.mqtt_password)
    if args.mqtt_tls:
        client.tls_set()
    client.connect(args.mqtt_host, args.mqtt_port)
    return client


class HomeAssistant:
    def __init__(self, url, token):
        ws_url = re.sub(r'^http', 'ws', url.rstrip('/')) + '/api/websocket'
        self.ws = connect(ws_url, max_size=None)
        self.id = 0
        self.ws.recv()  # auth_required
        self.ws.send(json.dumps({'type': 'auth', 'access_token': token}))
        if json.loads(self.ws.recv()).get('type') != 'auth_ok':
            sys.exit('Home Assistant authentication failed')

    def call(self, **msg):
        self.id += 1
        self.ws.send(json.dumps({'id': self.id, **msg}))
        while True:
            reply = json.loads(self.ws.recv())
            if reply.get('id') == self.id:
                if not reply.get('success'):
                    raise RuntimeError(reply.get('error'))
                return reply.get('result')

    def mqtt_entities(self):
        """Return {unique_id: entity_id} for all MQTT entities."""
        return {e['unique_id']: e['entity_id'] for e in self.call(type='config/entity_registry/list') if e['platform'] == 'mqtt'}

    def close(self):
        self.ws.close()


def snapshot(args):
    configs = read_discovery_configs(args)
    if not configs:
        sys.exit(f"No discovery configs with base '{args.base}' found under '{args.discovery_prefix}/'")

    ha = HomeAssistant(args.ha_url, args.ha_token)
    entities = ha.mqtt_entities()
    ha.close()

    for c in configs:
        c['entity_id'] = entities.get(c['uniq_id'])

    with open(args.file, 'w') as f:
        json.dump(configs, f, indent=2)

    found = sum(1 for c in configs if c['entity_id'])
    print_success(f'Saved {len(configs)} discovery configs ({found} with an HA entity) to {args.file}')


def apply(args):
    with open(args.file) as f:
        old_configs = [c for c in json.load(f) if c['entity_id']]

    current = {}
    for c in read_discovery_configs(args):
        current.setdefault(c['fingerprint'], []).append(c)

    ha = HomeAssistant(args.ha_url, args.ha_token)
    entities = ha.mqtt_entities()
    entity_ids = set(entities.values())

    renamed = unchanged = skipped = 0
    for old in old_configs:
        candidates = current.get(old['fingerprint'], [])
        stale = [c for c in candidates if c['uniq_id'] == old['uniq_id']]
        new = [c for c in candidates if c['uniq_id'] != old['uniq_id']]

        if not new:
            unchanged += 1
            continue
        if len(new) > 1:
            print_warn(f"Skipping {old['entity_id']}: {len(new)} new entities match ({', '.join(c['uniq_id'] for c in new)})")
            skipped += 1
            continue

        new_entity_id = entities.get(new[0]['uniq_id'])
        if not new_entity_id:
            print_warn(f"Skipping {old['entity_id']}: HA has no entity for new unique ID {new[0]['uniq_id']} yet")
            skipped += 1
            continue
        if new_entity_id == old['entity_id']:
            unchanged += 1
            continue

        old_entity_still_there = entities.get(old['uniq_id']) == old['entity_id']
        if old['entity_id'] in entity_ids and not old_entity_still_there:
            print_warn(f"Skipping {new_entity_id}: {old['entity_id']} is used by another entity")
            skipped += 1
            continue

        print(f"{new_entity_id} -> {old['entity_id']}")
        if args.dry_run:
            renamed += 1
            continue

        # clear the old retained discovery config, otherwise HA recreates the old entity
        if stale:
            client = mqtt_connect(args)
            client.loop_start()
            for c in stale:
                client.publish(c['topic'], b'', qos=1, retain=True).wait_for_publish()
            client.loop_stop()
            client.disconnect()

        if old_entity_still_there:
            deadline = time.time() + 10
            while time.time() < deadline and ha.mqtt_entities().get(old['uniq_id']):
                time.sleep(0.5)
            if ha.mqtt_entities().get(old['uniq_id']):
                ha.call(type='config/entity_registry/remove', entity_id=old['entity_id'])

        ha.call(type='config/entity_registry/update', entity_id=new_entity_id, new_entity_id=old['entity_id'])
        entity_ids.discard(new_entity_id)
        renamed += 1

    ha.close()

    action = 'Would rename' if args.dry_run else 'Renamed'
    print_success(f'{action} {renamed} entities, {unchanged} unchanged, {skipped} skipped')


def main():
    parser = argparse.ArgumentParser(description='Keep HA entity IDs when changing the EMS-ESP Entity ID format')
    parser.add_argument('step', choices=['snapshot', 'apply'])
    parser.add_argument('--dry-run', action='store_true', help='only show what apply would change')
    parser.add_argument('--file', default='ha_entity_snapshot.json', help='snapshot file')
    parser.add_argument('--ha-url', default='http://homeassistant.local:8123')
    parser.add_argument('--ha-token', default=os.environ.get('HA_TOKEN'))
    parser.add_argument('--mqtt-host', default='homeassistant.local')
    parser.add_argument('--mqtt-port', type=int, default=1883)
    parser.add_argument('--mqtt-user')
    parser.add_argument('--mqtt-password', default=os.environ.get('MQTT_PASSWORD'))
    parser.add_argument('--mqtt-tls', action='store_true')
    parser.add_argument('--base', default='ems-esp', help='EMS-ESP MQTT base')
    parser.add_argument('--discovery-prefix', default='homeassistant', help='HA MQTT discovery prefix')
    parser.add_argument('--wait', type=float, default=3, help='seconds to collect retained discovery configs')
    args = parser.parse_args()

    if not args.ha_token:
        sys.exit('An HA long-lived access token is required (--ha-token or HA_TOKEN)')

    try:
        snapshot(args) if args.step == 'snapshot' else apply(args)
    except (OSError, RuntimeError) as e:
        print_fail(f'Error: {e}')
        sys.exit(1)


if __name__ == '__main__':
    main()
