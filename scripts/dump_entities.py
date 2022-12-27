# use like
#   make clean; make ARGS=-DEMSESP_STANDALONE_DUMP; echo "test dump" | ./emsesp | python3 ./scripts/dump_entities.py 

import fileinput
with fileinput.input() as f_input:
    inRecordingMode = False
    for line in f_input:
        if not inRecordingMode:
            if line.startswith('---- CSV START ----'):
                inRecordingMode = True
        elif line.startswith('---- CSV END ----'):
            inRecordingMode = False
        else:
            print(line, end="") 
