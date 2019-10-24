import numpy as np

def count(M):
    memo = np.zeros((1,M.shape[0]))
    for i in range(0, M.shape[0]):
        ct = 0
        for j in range(0, M.shape[0]):
            if(i != j and M[i][j] != 0):
                ct += 1
        memo[i] = ct

    return memo

if __name__ == "__main__":

    B = np.loadtxt("bi.txt")
    ADJ = np.zeros((186, 1059))

    for edge in B:
        ADJ[int(edge[0]) - 1, int(edge[1]) - 1] = 1

    # left projection
    LP = np.dot(ADJ, ADJ.T)
    LCT = count(LP)

    # right projection
    RP = np.dot(ADJ.T, ADJ)
    RCT = count(RP)
