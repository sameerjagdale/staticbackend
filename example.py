def fn2(a: 'array(float64,1)' ,b: 'array(float64,1)' ,n: 'int32') -> 'void':
    for i in range(n):
        a[i] = b[i]
    return

