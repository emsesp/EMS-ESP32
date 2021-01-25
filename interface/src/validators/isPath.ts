const pathLengthRegex = /^[^.]{0,108}$/
const pathPatternRegex = /^([a-zA-Z0-9_][a-zA-Z0-9/_-]*[a-zA-Z0-9_])$/

export default function isPath(path: string) {
  return pathLengthRegex.test(path) && pathPatternRegex.test(path);
}