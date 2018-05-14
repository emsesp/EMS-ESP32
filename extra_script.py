Import("env")

#my_flags = env.ParseFlags(env['BUILD_FLAGS'])
#defines = {k: v for (k, v) in my_flags.get("CPPDEFINES")}
# print defines

#env.Replace(PROGNAME="firmware_%s" % defines.get("VERSION"))

# print env.Dump()

env.Replace(PROGNAME="firmware_%s" % env['BOARD'])

