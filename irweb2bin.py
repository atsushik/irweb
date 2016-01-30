import json
import sys

jsonLines = 0
for line in sys.stdin:
    if jsonLines > 0:
        sys.exit()
    if not line.startswith("{\""):
        continue
    #print line
    jsonLines += 1
    j = json.loads(line)
    #print j
    data = j[u'data']
    #print data
    #
    # http://elm-chan.org/docs/ir_format.html
    #print data[0], data[1], data[2]
    format = None
    T = 0
    if 3 < (float(data[0]) / float(data[1])):
        format = 'SONY'
        data = data[1:]
        T=float(data[0])/4.0
    elif (float(data[0]) / float(data[2])) < 12:
        format = 'AEHA'
        data = data[2:]
        T=float(data[0])/8.0
    elif 12 < (float(data[0]) / float(data[2])):
        format = 'NEC'
        data = data[2:]
        T=float(data[0])/16.0
    #print "FORMAT  : %s" % (format)
    #print "     T  : %.3f" % (T)
    #print "payload : (len=%d)" % (len(data)) , data
    cmdLen = 0
    cmdStr = ""
    cmd = 0
    if format == 'AEHA' or format == 'NEC':
        while 1 < len(data): 
            cmdLen += 1
            buf  = data[0:2]
            data = data[2:]
            cmd = cmd << 1
            if (float(buf[1]) / float(buf[0])) < 2.0:
                cmdStr = cmdStr+"0"
                cmd = cmd + 0
            else:
                cmdStr = cmdStr+"1"
                cmd = cmd + 1
    if format == 'SONY':
        while 1 < len(data):
            cmdLen += 1
            buf  = data[0:2]
            data = data[2:]
            cmd = cmd << 1
            if (float(buf[1]) / float(buf[0])) < 1.5:
                cmdStr = cmdStr+"0"
                cmd = cmd + 0
            else:
                cmdStr = cmdStr+"1"
                cmd = cmd + 1
    print "%s\t%s" % (format, cmdStr)
    #print "LENGTH=%d\t%s" % (cmdLen, cmdStr)
    #print "LENGTH=%d\t%x" % (cmdLen, cmd)
