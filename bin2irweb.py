import json
import sys

jsonLines = 0
for line in sys.stdin:
    [format, cmdStr] = line.split("\t")
    T = 0
    data = []
    if   format == u'AEHA':
        T = 425
        data = [8*T, 4*T]
    elif format == u'NEC':
        T = 562
        data = [16*T, 8*T]
    elif format == u'SONY':
        T = 600
        data = [4*T]
    else:
        sys.exit()
    #
    #print data
    while 0 < len( cmdStr ):
        if   format == u'AEHA' or format == u'NEC':
            if cmdStr[0] == u'0':
                data.append(T)
                data.append(T)
            if cmdStr[0] == u'1':
                data.append(T)
                data.append(3*T)
        elif format == u'SONY':
            if cmdStr[0] == u'0':
                data.append(T)
                data.append(T)
            if cmdStr[0] == u'1':
                data.append(T)
                data.append(2*T)
        cmdStr = cmdStr[1:] 
    if   format == u'AEHA' or format == u'NEC':
        data.append(T)
    #print len(data), data
    print json.dumps(
            {'format':'raw', 
             'freq':38,
             'len':len(data),
             'data':data
             })


