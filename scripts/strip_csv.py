# strips out lines between two markers
# pipe a file into it, for example: 'cat x | python3 strip_csv.py'
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
