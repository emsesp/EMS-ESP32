const hostnameLengthRegex = /^.{0,48}$/
const hostnamePatternRegex = /^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9-]*[a-zA-Z0-9])\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9-]*[A-Za-z0-9])$/

export default function isHostname(hostname: string) {
  return hostnameLengthRegex.test(hostname) && hostnamePatternRegex.test(hostname);
}
