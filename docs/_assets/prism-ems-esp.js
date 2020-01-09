Prism.languages.console = {
	'regex': {
		pattern: /(?<!\s)^\w+/im,
		lookback: true
	},
	'operator': /^\b(?:rule)\d?\b/im,
	'constant': /\b(?<=on)\s\S{1,}#\S{1,}\b/i,
	'selector': /(?<!(power\w+))\s(?:on|do|endon|break|if|else|elseif|and|or)\b/i,
	'boolean': /\b(?:backlog)\b/i,
	'comment': /[<]\w+[>]/,
	'deleted': /[;]\s/,
	'json': {
		pattern: /[{].*[}]/,
			alias: 'language-json',
			inside: Prism.languages.json
			},
};
