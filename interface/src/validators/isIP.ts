const ipAddressRegexp = /^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/;
const ipv6AddressRegexp = /^([a-fA-F0-9]{0,4}:([a-fA-F0-9]{0,4}:){1,6}[a-fA-F0-9]{0,4})$/;

export default function isIp(ipAddress: string) {
  return ipAddressRegexp.test(ipAddress) || ipv6AddressRegexp.test(ipAddress);
}
