import json
Import("env")


# print(env.Dump())
# print(env["PROGNAME"])
# my_flags = env.ParseFlags(env['BUILD_FLAGS'])
# print(my_flags)

# defines = {k: v for (k, v) in my_flags.get("CPPDEFINES")}
# print(defines.get("VERSION"))

env.Replace(PROGNAME="firmware")
# print(env["PROGNAME"])