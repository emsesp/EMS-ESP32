# Fault code packs: specification

The explanations of a manufacturer's fault codes (what a code means, the possible cause, what to do)
are **not** part of the firmware or the Web UI. They are **packs**: small JSON files that are installed
on the EMS-ESP gateway when needed. Anyone can write a pack for another manufacturer or language and
host it anywhere.

Status: implemented in the Web UI and the firmware API (see `src/web/WebFaultPackService.cpp`).

## 1. Terms

| Term | Meaning |
| --- | --- |
| Pack | One file with the explanations for **one brand in one language**, e.g. `buderus.de.json` |
| Installed pack | A pack stored on the gateway's file system, available to every browser, also without internet |

Not to be confused with the firmware *Modules* page (licensed code modules); packs only contain data.

## 2. How it works

1. The user downloads the pack of the manufacturer and language from a page that lists packs (the Web UI links
   to one; anybody can publish packs on any web page or repository).
2. In the Web UI (*Dashboard > Maintenance & faults*) the user installs the pack **from the file**.
3. The Web UI validates the file, compresses it and uploads it to the gateway, which stores it. The gateway
   needs no internet connection and no HTTPS client.
4. From then on every browser reads the pack from the gateway.
5. A fault card of a device uses the installed pack of the device's brand, in the Web UI language.

The pack is used for a device only when the brand matches, because the same code can mean something
completely different for another manufacturer. Without a matching pack the raw code is shown.

The Web UI does not download packs itself: its content security policy only lets the browser talk to the
gateway and GitHub's API, so a pack comes from a file the user picked.

## 3. Publishing packs

A pack is a plain file, so a list of packs can be any web page, a folder in a repository or a download area.
Name the files `<brand>.<lang>.json`. The packs published together with this specification are in
https://github.com/beabel/EMS-ESP-faultcodes, which also has a script that checks a pack against this format.

## 4. Pack file

```json
{
  "format": 1,
  "brand": "buderus",
  "lang": "de",
  "version": "2026-11-22",
  "source": "where the explanations come from",
  "license": "license of the text",
  "notice": "optional, e.g. a disclaimer",
  "applies": { "unbranded_with_boiler": true },
  "classes": {
    "14": { "title": "Class title", "meaning": "What the class means", "severity": "warn" }
  },
  "codes": [
    ["H08", 1018, "14", "Title", "Possible cause", "Remedy"]
  ]
}
```

- `brand`: lower case, exactly as the devices report their brand (e.g. `buderus`).
- `lang`: the language code of the Web UI (`en`, `de`, `fr`, `nl`, ...).
- `version`: any string that sorts by date, shown to the user, used to offer updates.
- `source` and `license` are required, so the origin of the texts is always visible. `notice` is optional text
  for the reader, for example that the explanations come without a guarantee.
- `applies.unbranded_with_boiler`: also use the pack for devices without a brand (e.g. a controller) in a
  system whose boiler has this brand.
- `applies.brands` (optional): more brand names, as the devices report them, for which the pack is used when brands
  share a table. Only a pack of the brand of the device is looked up on the gateway; the list is checked once that
  pack is loaded.
- `classes`: fault classes with a **severity**: `info`, `service`, `warn`, `error` or `unknown`.
  The severity is part of the pack, so every manufacturer can define its own classes.
- `codes`: `[code, additional code or null, class id, title, cause, remedy]`. The additional code is optional
  because not every manufacturer has one. Entries can be left empty (`""`).
  A row can have a 7th value, a flag: `"f"` = the text is the one of the original language because a translation is
  missing, `"m"` = machine translated and not reviewed, `""` = none. The Web UI shows a hint for `f` and `m`. An 8th
  value is the id of a **scope**.
- `scopes` (optional): the product series or devices some entries are for, because a code can mean something else on
  another series of the same brand: `{ "id": { "name": "...", "products": [123], "names": ["Cerapur"] } }`. `products`
  are the product ids of EMS-ESP, `names` are words in the device name (case does not matter). An entry with a scope is
  only used for a device that matches; an entry without a scope is used for every device of the brand. When entries of
  both kinds exist for a code, the ones that match a scope are shown. The code search shows all entries, with the name
  of the scope.
- Texts are plain text only (line breaks as `\n`), never HTML. The Web UI shows them as text.
- Unknown extra fields are ignored, so the format can grow.

## 5. Languages

- One file per brand and language: `buderus.de.json`, `buderus.en.json`, ...; a translator works on one file.
- The Web UI uses the pack in its own language; if there is none, it falls back to `en`, then to any pack
  of that brand.
- A translation has the same `codes` (code and additional code) as the original; only the texts change.
  An entry missing in a translation is shown from the fallback language.
- New languages are added as files; neither the firmware nor the Web UI changes.

## 6. Gateway storage and API (firmware)

Stored in the gateway's file system as `/faults/<brand>.<lang>.json` (the content is gzip compressed) with a
small `.meta` file next to it that holds the version and the number of entries. The browser compresses the
file before the upload, so a pack of 350 KB needs about 45 KB.

| Endpoint | Who | What |
| --- | --- | --- |
| `GET /rest/faultPacks` | signed-in users | list of installed packs (brand, lang, version, entries, size) and the free space |
| `GET /rest/faultPack?brand=..&lang=..` | signed-in users | the pack file |
| `POST /rest/uploadFaultPack?brand=..&lang=..&version=..&entries=..` | admin | upload a pack (multipart form, the compressed file) |
| `POST /rest/deleteFaultPack` `{"brand":..,"lang":..}` | admin | remove a pack |

Rules in the firmware:

- `brand` and `lang` only `a-z`, `0-9`, `-`; the file name is built from them, never taken from the upload.
- Upload size limited (proposal: 256 KB compressed); refused when the free space would drop too low.
- Written to a temporary file and renamed, so a failed upload leaves the old pack intact.
- Nothing is parsed or kept in RAM on the gateway; the file is stored and streamed as is.
- Not part of the settings backup (a pack can be installed again).

Boards: the file system is 2 MB on 16 MB boards and 4 MB on 32 MB boards. On 4 MB boards it is only 64 KB,
so the Web UI does not offer installing packs there.

## 7. Web UI

- *Dashboard > Maintenance & faults*: lists the installed packs (admins can remove them), installs a pack
  from a file and links to a list of packs. When a device with a known brand reports a fault code and no pack
  is installed, the fault card says so.
- The Web UI validates every file before uploading it: `format`, `brand`, `lang`, `version`, size, the
  structure of `classes` and `codes`.

## 8. Security

- Packs are data only, shown as text; nothing from a pack is executed or inserted as HTML.
- Installing and removing needs admin rights; the gateway checks size and names itself.
- The brand, language, version and number of entries of an installed pack are shown to the user.

## 9. Open points

- Origin and license of the Buderus explanations (`source` / `license` in the pack); to be clarified before
  any repository hosts the file.
- Which page the Web UI links to as the list of packs (`FAULT_PACK_LIST_URL` in `faultPacks.ts`).
- Downloading a pack from a web address (needs the content security policy to allow it, or the gateway to
  download it itself).
- The firmware sends the stored file with `Content-Encoding: gzip`; to be confirmed on a real gateway.
- The free space on real devices (settings and other files share the file system).
- Per-device settings, such as hiding fault cards, are not part of this.
