import getopt, sys

usage = "[Usage] LogToTestbench.py -i log.txt -o testbench.txt"
clean_style = False

def getOptions():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "i:o:c", [])
    except:
        print usage
        sys.exit(1)
    
    global clean_style 

    ifn = None
    ofn = None

    for o,a in opts:
        if o == '-i':
            ifn = a
        elif o == '-o':
            ofn = a
        elif o == '-c':
            clean_style = True
        else:
            print "[Warning] Unknown option: ", o
    
    if ifn == None or ofn == None:
        print usage
        sys.exit(1)
    
    return (ifn,ofn)


if "__main__"==__name__:
    (input_filename, output_filename) = getOptions()

    separator = "\r\n"
    if clean_style:
        separator = "\t"
    line = ""
    testline = ""

    try:
        log_file = open(input_filename, "r")
        
        player_size = int(log_file.readline())
        if not clean_style:
            testline = testline + "# "+str(player_size)+" players\r\n"

        if not clean_style:
            testline = testline + "# bid price\r\n"
        line = log_file.readline()
        testline = testline + line
        bid_price_list = []
        for s in line.split():
            bid_price_list.append(int(s))
        
        if not clean_style:
            testline = testline + "# bid position\r\n"
        line = log_file.readline()
        bid_position_list = line.split()
        for _ in range(player_size):
            id = bid_price_list.index(max(bid_price_list))
            bid_price_list[id] = -1
            testline = testline + " " + bid_position_list[id*2] + " " + bid_position_list[id*2+1] + " # P" + str(id) + "\r\n"

        round = 0
        while True:
            line = log_file.readline()
            if line == "":
                break
            
            round = round + 1
            testline = testline + "\r\n"
            if not clean_style:
                testline = testline + "# Round "+str(round)+"\r\n"

            command_list = []
            for s in line.split():
                command_list.append(int(s))
            if (len(command_list) < (player_size)*(player_size+3)):
                break

            t = 0
            for id in range(player_size):
                if not clean_style:
                    testline = testline + "# P" + str(id) + separator
                for q in range(player_size):
                    if q != id:
                        testline = testline + " " + str(command_list[t])
                    t = t+1
                testline = testline + separator
                while command_list[t] != -1:
                    testline = testline + " " + str(command_list[t])
                    t = t+1
                t = t+1
                testline = testline + " -1" + separator
                for _ in range(2):
                    testline = testline + " " + str(command_list[t])
                    t = t+1
                testline = testline + separator

    except:
        print "[Error] failed to load..."
    finally:
        log_file.close()

    try:
        testbench_file = open(output_filename, "w")
        testbench_file.write(testline)
    except:
        print "[Error] output failed."
    finally:
        testbench_file.close()