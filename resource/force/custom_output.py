import sys

output = ""
no_new_line = False
escape_char = False

for argv in sys.argv[1:] :

    if argv.startswith("-") :
        
        for p in argv :
            if p == "e" : escape_char = True
            elif p == "n" : no_new_line = True
            else :
                RuntimeError("Unknown param: -{p}"
                            "Usage: {sys.argv[0]} -n|-e <output>")
                

    else :

        output = argv.encode('utf-8').decode('unicode_escape') if escape_char else argv
        break


print(output, end = "" if no_new_line else "\n")