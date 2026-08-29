"""
Utility script counting and generating gap sequences for Shellsort research.
Either all possible valid sequences, or only f1 sequences, which are sequences that satisfy the condition:
    downBoundary * previous_gap <= current_gap <= upBoundary * previous_gap
"""

def CountSomeSequences(N, lo, hi):
    memo = {}
    def f(v):
        if v not in memo:
            lo_bound = int(v * lo) if v * lo == int(v * lo) else int(v * lo) + 1
            hi_bound = min(int(v * hi), N - 1)
            memo[v] = 1 + sum(f(w) for w in range(lo_bound, hi_bound + 1))
        return memo[v]
    return f(1)


def GetAllSequences(N):
    sequences = []
    
    def GenerateSequence(current_seq, next_num):
        if next_num >= N:
            sequences.append(current_seq[:])
            return
        
        # Branch 1: Don't include next_num
        GenerateSequence(current_seq, next_num + 1)
        
        # Branch 2: Include next_num
        current_seq.append(next_num)
        GenerateSequence(current_seq, next_num + 1)
        current_seq.pop()
    
    GenerateSequence([1], 2)
    return sequences


def GetSomeSequences(N, downBoundary, upBoundary):
    sequences = []
    
    def GenerateSequence(current_seq, next_num):
        if next_num >= N:
            sequences.append(current_seq[:])
            return
        
        # Branch 1: Don't include next_num
        GenerateSequence(current_seq, next_num + 1)
        
        # Branch 2: Include next_num if it's within the boundaries
        if downBoundary*current_seq[-1] <= next_num <= upBoundary*current_seq[-1]:
            current_seq.append(next_num)
            GenerateSequence(current_seq, next_num + 1)
            current_seq.pop()
    
    GenerateSequence([1], 2)
    return sequences


if __name__ == "__main__":
    N = 30
    downBoundary = 1.1
    upBoundary = 10

    countAll = 2**(N-2)
    countSome = CountSomeSequences(N, downBoundary, upBoundary)

    print(f"Count All: {countAll} ~ {countAll:.5e}")
    print(f"Count Some: {countSome} ~ {countSome:.5e}")

    all_sequences = GetAllSequences(N)
    some_sequences = GetSomeSequences(N, downBoundary, upBoundary)
    print(f"Total sequences for N={N}: {len(all_sequences)}")
    print(f"Total some sequences for N={N}: {len(some_sequences)}")