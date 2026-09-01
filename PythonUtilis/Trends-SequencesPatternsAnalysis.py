import os
import re
from datetime import datetime
from collections import Counter
import matplotlib.pyplot as plt
import numpy as np

# --- CONFIGURATION ---
TARGET_ALGO = "Trends Analysis"
RESULTS_DIR = "../Results/FinalSets/Criterion-Comparisons/"  # Change to the directory path with your files
OUTPUT_DIR = f'outputs/Trends-SequencesPatternsAnalysis/{datetime.now().strftime("%Y-%m-%d_%H-%M-%S")}'

# Dictionary of input files. 
# Key: Sorting range size (N) or text identifier, Value: file path.
FILES = {
    "100": "CandidateGapSequences_100.txt",
    "250": "CandidateGapSequences_250.txt",
    "500": "CandidateGapSequences_500.txt",
    "750": "CandidateGapSequences_750.txt",
    "1000": "CandidateGapSequences_1000.txt",
    "2000": "CandidateGapSequences_2000.txt",
    "3000": "CandidateGapSequences_3000.txt",
    "4000": "CandidateGapSequences_4000.txt",
    "5000": "CandidateGapSequences_5000.txt",
    "6000": "CandidateGapSequences_6000.txt",
    "7000": "CandidateGapSequences_7000.txt",
    "8000": "CandidateGapSequences_8000.txt",
    "9000": "CandidateGapSequences_9000.txt",
    "10000": "CandidateGapSequences_10000.txt",
    "100000": "CandidateGapSequences_100000.txt",
    "Merge": "CandidateGapSequences_Merge.txt",
    "Supreme": "GapSequences_Supreme.txt",
}

# --- BASE LOGIC ---
def parse_file(filepath):
    """
    Parses the results file containing the format: ID|Type|Metadata: sequence.
    """
    sequences = []
    with open(filepath, 'r') as f:
        for line in f:
            line = line.strip()
            if line == "###":
                break  # End of relevant data
            if not line or ':' not in line: 
                continue
            
            header, seq_part = line.split(':', 1)
            sequence = [int(x) for x in seq_part.split() if x.isdigit()]
            
            if sequence:
                sequences.append(sequence)
    return sequences

def analyze_sequences(sequences, range_val_str):
    """Calculates required statistics: ratios, lengths, and bounds."""
    if not sequences:
        return None

    # 1. Ratio between gaps (bounds & most common)
    ratios = []
    for s in sequences:
        # Loop up to len(s) - 2 to skip the ratio involving the final gap (usually 1)
        for i in range(len(s) - 2):
            if s[i+1] > 0:
                ratios.append(s[i] / s[i+1])
                
    rounded_ratios = [round(r, 2) for r in ratios]
    ratio_counter = Counter(rounded_ratios)
    
    ratio_stats = {
        'min': min(ratios) if ratios else 0,
        'max': max(ratios) if ratios else 0,
        'avg': np.mean(ratios) if ratios else 0,
        'most_common': ratio_counter.most_common(5),
        'all_ratios': ratios
    }

    # 2. Length of sequence & Initial Gap (Largest gap) stats
    lengths = [len(s) for s in sequences]
    largest_gaps = [s[0] for s in sequences] # Starting element is the largest gap
    
    length_stats = {
        'min_len': min(lengths),
        'max_len': max(lengths),
        'avg_len': np.mean(lengths),
        'most_common_len': Counter(lengths).most_common(3)
    }
    
    gap_bound_stats = {
        'min_largest_gap': min(largest_gaps) if largest_gaps else 0,
        'max_largest_gap': max(largest_gaps) if largest_gaps else 0,
        'avg_largest_gap': np.mean(largest_gaps) if largest_gaps else 0
    }
    
    # 3. Knuth Bounds (Only if range is a number)
    if range_val_str.isdigit():
        range_n = int(range_val_str)
        knuth_bound = range_n / 3.0
        
        gap_bound_stats.update({
            'knuth_bound_diff': [g - knuth_bound for g in largest_gaps],
            'avg_ratio_to_range': np.mean([g / range_n for g in largest_gaps]),
            'knuth_target': knuth_bound
        })

    return {
        'ratios': ratio_stats,
        'lengths': length_stats,
        'bounds': gap_bound_stats,
        'num_seqs': len(sequences)
    }

# --- REPORTING ---
def export_to_txt(results_dict):
    """Generates a text report for all ranges."""
    for range_val, data in results_dict.items():
        filepath = os.path.join(OUTPUT_DIR, f'Analysis_Range_{range_val}.txt')
        with open(filepath, 'w') as f:
            f.write(f"=== REPORT FOR RANGE {range_val} ({data['num_seqs']} sequences) ===\n\n")
            
            f.write("1. RATIO BETWEEN GAPS (Excluding final gap):\n")
            f.write(f"  Min: {data['ratios']['min']:.4f}\n")
            f.write(f"  Max: {data['ratios']['max']:.4f}\n")
            f.write(f"  Average: {data['ratios']['avg']:.4f}\n")
            f.write("  Most common ratios (precision 0.01):\n")
            for val, count in data['ratios']['most_common']:
                f.write(f"    Ratio {val}: {count} occurrences\n")

            if 'knuth_target' in data['bounds']:
                f.write("\n2. LENGTH & BOUNDS (Knuth's Theory N/3):\n")
                f.write(f"  Sequence length - Min: {data['lengths']['min_len']}, Max: {data['lengths']['max_len']}, Average: {data['lengths']['avg_len']:.1f}\n")
                f.write(f"  Smallest largest gap: {data['bounds']['min_largest_gap']}\n")
                f.write(f"  Average largest gap: {data['bounds']['avg_largest_gap']:.1f}\n")
                f.write(f"  Biggest largest gap: {data['bounds']['max_largest_gap']}\n")
                f.write(f"  Knuth's target (N/3): {data['bounds']['knuth_target']:.1f}\n")
                f.write(f"  Average fraction of range N (Expected ~0.333): {data['bounds']['avg_ratio_to_range']:.4f}\n")
            else:
                f.write("\n2. LENGTH & LARGEST GAPS:\n")
                f.write(f"  Sequence length - Min: {data['lengths']['min_len']}, Max: {data['lengths']['max_len']}, Average: {data['lengths']['avg_len']:.1f}\n")
                f.write(f"  Smallest largest gap: {data['bounds']['min_largest_gap']}\n")
                f.write(f"  Average largest gap: {data['bounds']['avg_largest_gap']:.1f}\n")
                f.write(f"  Biggest largest gap: {data['bounds']['max_largest_gap']}\n")
                f.write(f"  (Knuth's bounds analysis skipped for non-numeric range name)\n")

# --- CHARTS ---
def plot_ratios(results_dict):
    """Histogram of ratios between gaps."""
    plt.figure(figsize=(10, 6))
    
    sorted_items = sorted(results_dict.items(), key=lambda x: len(x[1]['ratios']['all_ratios']), reverse=True)
    
    for range_val, data in sorted_items:
        ratios = data['ratios']['all_ratios']
        ratios = [r for r in ratios if r < 5.0] 
        plt.hist(ratios, bins=50, alpha=0.75, label=f'Range {range_val}', edgecolor='black')
        
    plt.title('Distribution of Gaps Ratios (Absolute Counts, Excluding Final Gap)')
    plt.xlabel('Gap Ratio (h[i] / h[i+1])')
    plt.ylabel('Frequency')
    plt.legend()
    plt.grid(alpha=0.3)
    plt.savefig(f'{OUTPUT_DIR}/plot_ratios_absolute.png')
    plt.close()

def plot_ratios_normalized(results_dict):
    """Normalized histogram of ratios between gaps."""
    plt.figure(figsize=(10, 6))
    
    sorted_items = sorted(results_dict.items(), key=lambda x: len(x[1]['ratios']['all_ratios']), reverse=True)
    
    for range_val, data in sorted_items:
        ratios = data['ratios']['all_ratios']
        ratios = [r for r in ratios if r < 5.0] 
        
        # density=True normalizes the data so the area under the histogram equals 1
        plt.hist(ratios, bins=50, alpha=0.6, label=f'Range {range_val}', edgecolor='black', density=True)
        
    plt.title('Normalized Distribution of Gaps Ratios (Excluding Final Gap)')
    plt.xlabel('Gap Ratio (h[i] / h[i+1])')
    plt.ylabel('Density (Relative Frequency)')
    plt.legend()
    plt.grid(alpha=0.3)
    plt.savefig(f'{OUTPUT_DIR}/plot_ratios_normalized.png')
    plt.close()

def plot_knuth_bounds(results_dict):
    """Chart of the relationship of the largest gap to Knuth's bound (N/3)."""
    numeric_keys = [k for k in results_dict.keys() if k.isdigit()]
    if not numeric_keys:
        return
        
    ranges = sorted([int(k) for k in numeric_keys])
    
    avg_gaps = [results_dict[str(r)]['bounds']['avg_largest_gap'] for r in ranges]
    knuth_bounds = [r / 3.0 for r in ranges]
    
    plt.figure(figsize=(10, 6))
    plt.plot(ranges, avg_gaps, marker='o', linestyle='-', linewidth=2, label='Average largest generated gap')
    plt.plot(ranges, knuth_bounds, marker='x', linestyle='--', linewidth=2, color='red', label="Knuth's bound (N/3)")
    
    plt.title('Relationship of the largest gap to the sorting range')
    plt.xlabel('Range size (N)')
    plt.ylabel('Value of the initial gap (Largest Gap)')
    plt.legend()
    plt.grid(alpha=0.3)
    plt.savefig(f'{OUTPUT_DIR}/plot_knuth_bounds.png')
    plt.close()

def plot_largest_gaps_stats(results_dict):
    """Plot of the Smallest, Average, and Biggest initial gap for all files."""
    # Sort keys: numeric strings numerically first, then alphabetical strings
    def sort_key(k):
        return (0, int(k)) if k.isdigit() else (1, k)
        
    categories = sorted(results_dict.keys(), key=sort_key)
    
    mins = [results_dict[k]['bounds']['min_largest_gap'] for k in categories]
    avgs = [results_dict[k]['bounds']['avg_largest_gap'] for k in categories]
    maxs = [results_dict[k]['bounds']['max_largest_gap'] for k in categories]
    
    plt.figure(figsize=(14, 7))
    
    plt.plot(categories, maxs, marker='^', linestyle='-', color='red', label='Biggest Gap')
    plt.plot(categories, avgs, marker='o', linestyle='-', color='blue', label='Average Gap')
    plt.plot(categories, mins, marker='v', linestyle='-', color='green', label='Smallest Gap')
    
    # Annotate values directly on the plot
    y_max = max(maxs) if maxs else 100
    offset = y_max * 0.02
    
    for i, cat in enumerate(categories):
        plt.text(i, maxs[i] + offset, f"{maxs[i]:.0f}", ha='center', va='bottom', fontsize=8, color='darkred')
        plt.text(i, avgs[i] + offset, f"{avgs[i]:.0f}", ha='center', va='bottom', fontsize=8, color='darkblue')
        plt.text(i, mins[i] - offset, f"{mins[i]:.0f}", ha='center', va='top', fontsize=8, color='darkgreen')
        
    plt.title('Smallest, Average, and Biggest Initial Gap (First Element)')
    plt.xlabel('File / Range')
    plt.ylabel('Gap Value')
    plt.xticks(rotation=45)
    
    # Increase y-limit slightly to fit the top annotations
    plt.ylim(bottom=-offset*2, top=y_max + (offset * 3))
    plt.legend()
    plt.grid(alpha=0.3)
    plt.tight_layout()
    plt.savefig(f'{OUTPUT_DIR}/plot_largest_gaps_stats.png')
    plt.close()

# --- MAIN LOOP ---
def main():
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    all_results = {}
    
    for range_val, filepath in FILES.items():
        full_path = os.path.join(RESULTS_DIR, filepath)
        print(f"Processing range: {range_val}...")
        
        if not os.path.exists(full_path):
            print(f"File not found: {full_path}")
            continue
            
        sequences = parse_file(full_path)
        results = analyze_sequences(sequences, range_val)
        
        if results:
            all_results[range_val] = results
            
    if all_results:
        export_to_txt(all_results)
        plot_ratios(all_results)
        plot_ratios_normalized(all_results)
        plot_knuth_bounds(all_results)
        plot_largest_gaps_stats(all_results)
        print(f"Analysis complete. Results saved in: {OUTPUT_DIR}")
    else:
        print("No data processed.")

if __name__ == "__main__":
    main()