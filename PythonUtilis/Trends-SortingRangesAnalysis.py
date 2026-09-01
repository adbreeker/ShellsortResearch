import os
import re
import math
from datetime import datetime
from collections import Counter
import matplotlib.pyplot as plt
import numpy as np

# --- CONFIGURATION ---

# The algorithm being analyzed (for titles/labels)
TARGET_ALGO = "Final datasets — Comparisons"

RESULTS_DIR = "../Results/FinalSets/Criterion-Comparisons/"

# Maximum number of populations to analyze (e.g., 500000). 
# Set to None to analyze all populations in the files.
MAX_POPULATIONS = None

# Maximum number of sequences to analyze (e.g., 1000). 
# Set to None to analyze all sequences.
MAX_SEQUENCES = 100 

# Add as many files as you want here. 
# Key: Range size (will be sorted numerically) or custom string (will be placed at the end)
# Value: Filename
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

current_time = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
OUTPUT_DIR = f'outputs/Trends-SortingRangesAnalysis/{current_time}'

# ---------------------

def get_sorted_ranges(keys):
    """Sort numeric ranges numerically, and append custom string names sorted alphabetically."""
    numeric = sorted([k for k in keys if k.isdigit()], key=int)
    non_numeric = sorted([k for k in keys if not k.isdigit()])
    return [str(n) for n in numeric] + non_numeric

def get_dynamic_colors(num_colors):
    """Generate a dynamic colormap based on the number of files."""
    colormap = plt.colormaps['viridis']
    return [colormap(i) for i in np.linspace(0.2, 0.9, num_colors)]

def parse_file(filepath, max_pops=None, max_seqs=None):
    """Parse a results file and extract sequence data and metadata up to limits."""
    sequences = []
    with open(filepath, 'r') as f:
        for line in f:
            line = line.strip()
            if line == "###":
                break  # Stop parsing at the end marker
            if not line or ':' not in line: 
                continue
            
            header, seq_part = line.split(':', 1)
            
            # Check strictly for the format: {population}|{type}|{index}... if it exists
            match = re.match(r'^(\d+)\|', header)
            population = int(match.group(1)) if match else 0
            
            # Skip if population exceeds the defined limit
            if max_pops is not None and population > max_pops:
                continue
            
            header_parts = header.split('|')
            type_val = header_parts[1].strip() if len(header_parts) > 1 else "Unknown"
            
            is_mutated = 'Mutated' in header
            is_validated = 'Validated' in header
            
            sequence = [int(x) for x in seq_part.split() if x.isdigit()]
            
            if sequence:
                sequences.append({
                    'pop': population,
                    'type': type_val,
                    'mut': is_mutated,
                    'val': is_validated,
                    'seq': sequence
                })
                
                if max_seqs is not None and len(sequences) >= max_seqs:
                    break
                    
    return sequences

def analyze_sequences(sequences):
    """Perform analysis on parsed sequences."""
    if not sequences:
        return None
    
    raw_populations = [s['pop'] for s in sequences]
    seqs = [s['seq'] for s in sequences]
    num_sequences = len(sequences)
    
    # ---------------------------------------------------------
    # Calculate Cumulative Populations & Interval Statistics
    # ---------------------------------------------------------
    cumulative_pops = []
    base_pop = 0
    prev_pop = 0
    
    for p in raw_populations:
        # If the population drops, it means a new run was concatenated
        if p < prev_pop:
            base_pop += prev_pop
        
        cumulative_p = base_pop + p
        cumulative_pops.append(cumulative_p)
        prev_pop = p
        
    if len(cumulative_pops) > 1 and max(cumulative_pops) > 0:
        intervals = [cumulative_pops[i+1] - cumulative_pops[i] for i in range(len(cumulative_pops)-1)]
        avg_interval = np.mean(intervals)
        med_interval = np.median(intervals)
        min_interval = np.min(intervals)
        max_interval = np.max(intervals)
        std_interval = np.std(intervals)
    else:
        avg_interval = med_interval = min_interval = max_interval = std_interval = 0
        
    # Analyze sequences aligned from the END
    max_len = max(len(s) for s in seqs)
    padded_seqs = []
    
    for s in seqs:
        # Pad at the front to right-align the sequences (align by their endings)
        padded = [None] * (max_len - len(s)) + s
        padded_seqs.append(padded)
        
    avg_seq, min_seq, max_seq = [], [], []
    gap_freqs_by_pos = {}
    
    for pos in range(max_len):
        values = [s[pos] for s in padded_seqs if s[pos] is not None]
        if values:
            avg_seq.append(np.mean(values))
            min_seq.append(min(values))
            max_seq.append(max(values))
            
            # Position from the end (last element = 1)
            pos_from_end = max_len - pos
            counter = Counter(values)
            gap_freqs_by_pos[pos_from_end] = {
                'counts': counter,
                'most_common': counter.most_common(10),
                'total': len(values)
            }
        else:
            avg_seq.append(0)
            min_seq.append(0)
            max_seq.append(0)
            
    # Calculate Common Endings
    beginning_counts = {}
    for n in range(1, max_len + 1):
        beginnings = []
        for s in seqs:
            if len(s) >= n:
                beginnings.append(tuple(s[-n:]))
                
        if beginnings:
            counter = Counter(beginnings)
            most_common = counter.most_common(10)
            beginning_counts[n] = {
                'counts': counter,
                'most_common': most_common,
                'total': len(beginnings)
            }
            
    # Calculate Metadata Stats (Types, Mutated, Validated)
    types_counter = Counter(s['type'] for s in sequences)
    mut_count = sum(1 for s in sequences if s['mut'])
    val_count = sum(1 for s in sequences if s['val'])
            
    return {
        'num_sequences': num_sequences,
        'avg_interval': avg_interval,
        'med_interval': med_interval,
        'min_interval': min_interval,
        'max_interval': max_interval,
        'std_interval': std_interval,
        'avg_seq': avg_seq,
        'min_seq': min_seq,
        'max_seq': max_seq,
        'gap_freqs_by_pos': gap_freqs_by_pos,
        'beginning_counts': beginning_counts,
        'types_counter': types_counter,
        'mut_count': mut_count,
        'val_count': val_count,
        'populations': cumulative_pops
    }

def create_grid_subplots(num_items):
    """Creates a strictly 3-column grid based on the number of items."""
    cols = 3
    rows = math.ceil(num_items / cols)
    
    fig, axes = plt.subplots(rows, cols, figsize=(18, 6 * rows))
    
    if rows > 1:
        axes = axes.flatten()
    else:
        axes = axes.flatten() if isinstance(axes, np.ndarray) else [axes]
        
    return fig, axes, rows, cols

def format_title(range_val):
    """Helper to format titles cleanly for both numeric and custom ranges."""
    return f'Range {range_val}' if range_val.isdigit() else str(range_val)

def plot_num_sequences(all_results, sorted_ranges, colors, limit_label):
    """Plot 1: Number of sequences per range."""
    # Dynamic width up to 16 inches for readability with many files
    fig_width = max(12, len(sorted_ranges) * 1.0)
    fig, ax = plt.subplots(figsize=(fig_width, 7))
    
    counts = [all_results[r]['num_sequences'] for r in sorted_ranges]
    labels = [format_title(r) for r in sorted_ranges]
    
    bars = ax.bar(labels, counts, color=colors, edgecolor='black', linewidth=1.2)
    
    max_count = max(counts) if counts else 1
    offset = max_count * 0.02
    
    for bar, count in zip(bars, counts):
        ax.text(bar.get_x() + bar.get_width()/2, bar.get_height() + offset,
                str(count), ha='center', va='bottom', fontsize=10, fontweight='bold')
                
    ax.set_ylabel('Number of Sequences', fontsize=12)
    ax.set_xlabel('Dataset Category', fontsize=12)
    ax.set_title(f'Number of Gap Sequences Generated ({TARGET_ALGO}){limit_label}', fontsize=14, fontweight='bold')
    ax.set_ylim(0, max_count * 1.15)
    
    plt.xticks(rotation=45, ha='right', fontsize=11)
    plt.tight_layout()
    plt.savefig(f'{OUTPUT_DIR}/plot1_num_sequences.png', dpi=150)
    plt.close()

def plot_avg_interval(all_results, sorted_ranges, colors, limit_label):
    """Plot 2: Average population interval between sequences (with Std Dev)."""
    
    # Strictly filter out non-numeric custom sets (Merge, Supreme, etc.)
    numeric_ranges = [r for r in sorted_ranges if r.isdigit()]
    if not numeric_ranges:
        return
        
    # Match colors so they remain consistent with Plot 1
    filtered_colors = [colors[sorted_ranges.index(r)] for r in numeric_ranges]
    
    fig_width = max(12, len(numeric_ranges) * 1.0)
    fig, ax = plt.subplots(figsize=(fig_width, 7))
    
    intervals = [all_results[r]['avg_interval'] for r in numeric_ranges]
    stds = [all_results[r]['std_interval'] for r in numeric_ranges]
    labels = [format_title(r) for r in numeric_ranges]
    
    bars = ax.bar(labels, intervals, color=filtered_colors, edgecolor='black', linewidth=1.2)
    
    max_interval = max(intervals) if intervals else 1
    offset = max_interval * 0.02
    
    for bar, interval, std in zip(bars, intervals, stds):
        ax.text(bar.get_x() + bar.get_width()/2, bar.get_height() + offset,
                f'{interval:.0f}\n±{std:.0f}', ha='center', va='bottom', fontsize=9, fontweight='bold')
                
    ax.set_ylabel('Average Population Interval', fontsize=12)
    ax.set_title(f'Average Populations Between Finding New Sequences ({TARGET_ALGO}){limit_label}', fontsize=14, fontweight='bold')
    ax.set_ylim(0, max_interval * 1.25)
    
    plt.xticks(rotation=45, ha='right', fontsize=11)
    plt.tight_layout()
    plt.savefig(f'{OUTPUT_DIR}/plot2_avg_interval.png', dpi=150)
    plt.close()

def plot_common_beginnings(all_results, sorted_ranges, colors, limit_label):
    """Plot 4: Most common sequence endings (from 1) in a flexible 3-column grid."""
    num_ranges = len(sorted_ranges)
    
    for n in range(1, 7):
        fig, axes, rows, cols = create_grid_subplots(num_ranges)
        
        for idx, range_val in enumerate(sorted_ranges):
            ax = axes[idx]
            results = all_results[range_val]
            title = format_title(range_val)
            
            if n in results['beginning_counts']:
                data = results['beginning_counts'][n]
                if data['most_common']:
                    top_seqs = data['most_common'][:min(10, len(data['most_common']))]
                    
                    labels = [', '.join(map(str, seq)) for seq, _ in top_seqs][::-1]
                    percentages = [(count / data['total']) * 100 for _, count in top_seqs][::-1]
                    counts = [count for _, count in top_seqs][::-1]
                    
                    y_pos = np.arange(len(labels))
                    
                    bars = ax.barh(y_pos, percentages, color=colors[idx], 
                                   edgecolor='black', linewidth=0.8, height=0.7)
                                   
                    ax.set_yticks(y_pos)
                    ax.set_yticklabels(labels, fontsize=10)
                    ax.set_xlabel('Percentage (%)', fontsize=11)
                    ax.set_title(title, fontsize=13, fontweight='bold')
                    
                    max_pct = max(percentages) if percentages else 10
                    x_limit = max(max_pct * 1.3, 5)
                    ax.set_xlim(0, x_limit)
                    ax.grid(axis='x', alpha=0.3)
                    
                    half_axis = x_limit / 2
                    for bar, pct, cnt in zip(bars, percentages, counts):
                        width = bar.get_width()
                        if width >= half_axis:
                            ax.text(width - 0.5, bar.get_y() + bar.get_height()/2,
                                   f'{pct:.1f}% ({cnt})', ha='right', va='center',
                                   fontsize=9, fontweight='bold', color='white')
                        else:
                            ax.text(width + 0.3, bar.get_y() + bar.get_height()/2,
                                   f'{pct:.1f}% ({cnt})', ha='left', va='center',
                                   fontsize=9, fontweight='bold', color='black')
            else:
                ax.text(0.5, 0.5, "No data for this length", ha='center', va='center', transform=ax.transAxes)
                ax.set_title(title, fontsize=13, fontweight='bold')
                
        for idx in range(num_ranges, rows * cols):
            axes[idx].set_visible(False)
            
        plt.suptitle(f'{TARGET_ALGO}: Most Common Endings - Last {n} Element{"s" if n > 1 else ""} (from 1){limit_label}', 
                     fontsize=16, fontweight='bold')
        plt.tight_layout(rect=[0, 0.02, 1, 0.96])
        plt.savefig(f'{OUTPUT_DIR}/plot4_endings_length_{n}.png', dpi=150, bbox_inches='tight')
        plt.close()

def plot_common_gaps_by_pos(all_results, sorted_ranges, colors, limit_label):
    """Plot 5: Most common individual gaps at each position (from 1) in a 3-column grid."""
    num_ranges = len(sorted_ranges)
    
    for pos in range(1, 7):
        fig, axes, rows, cols = create_grid_subplots(num_ranges)
        
        for idx, range_val in enumerate(sorted_ranges):
            ax = axes[idx]
            results = all_results[range_val]
            title = format_title(range_val)
            
            if pos in results['gap_freqs_by_pos']:
                data = results['gap_freqs_by_pos'][pos]
                if data['most_common']:
                    top_gaps = data['most_common'][:min(10, len(data['most_common']))]
                    
                    labels = [f"{gap}" for gap, _ in top_gaps][::-1]
                    percentages = [(count / data['total']) * 100 for _, count in top_gaps][::-1]
                    counts = [count for _, count in top_gaps][::-1]
                    
                    y_pos = np.arange(len(labels))
                    
                    bars = ax.barh(y_pos, percentages, color=colors[idx], 
                                   edgecolor='black', linewidth=0.8, height=0.7)
                                   
                    ax.set_yticks(y_pos)
                    ax.set_yticklabels(labels, fontsize=10)
                    ax.set_xlabel('Percentage (%)', fontsize=11)
                    ax.set_title(title, fontsize=13, fontweight='bold')
                    
                    max_pct = max(percentages) if percentages else 10
                    x_limit = max(max_pct * 1.3, 5)
                    ax.set_xlim(0, x_limit)
                    ax.grid(axis='x', alpha=0.3)
                    
                    half_axis = x_limit / 2
                    for bar, pct, cnt in zip(bars, percentages, counts):
                        width = bar.get_width()
                        if width >= half_axis:
                            ax.text(width - 0.5, bar.get_y() + bar.get_height()/2,
                                   f'{pct:.1f}% ({cnt})', ha='right', va='center',
                                   fontsize=9, fontweight='bold', color='white')
                        else:
                            ax.text(width + 0.3, bar.get_y() + bar.get_height()/2,
                                   f'{pct:.1f}% ({cnt})', ha='left', va='center',
                                   fontsize=9, fontweight='bold', color='black')
            else:
                ax.text(0.5, 0.5, "No data for this position", ha='center', va='center', transform=ax.transAxes)
                ax.set_title(title, fontsize=13, fontweight='bold')
                
        for idx in range(num_ranges, rows * cols):
            axes[idx].set_visible(False)
            
        plt.suptitle(f'{TARGET_ALGO}: Most Common Individual Gaps - Position {pos} (from 1){limit_label}', 
                     fontsize=16, fontweight='bold')
        plt.tight_layout(rect=[0, 0.02, 1, 0.96])
        plt.savefig(f'{OUTPUT_DIR}/plot5_common_gaps_pos_{pos}.png', dpi=150, bbox_inches='tight')
        plt.close()

def plot_common_beginnings_detailed(all_results, sorted_ranges, colors, limit_label):
    """Plot 6: Summary plot showing coverage trends across all ranges in a 3-column grid."""
    num_ranges = len(sorted_ranges)
    fig, axes, rows, cols = create_grid_subplots(num_ranges)
    
    for idx, range_val in enumerate(sorted_ranges):
        ax = axes[idx]
        results = all_results[range_val]
        
        n_values = list(range(1, 7))
        top1_percentages, top3_percentages, top5_percentages = [], [], []
        
        for n in n_values:
            if n in results['beginning_counts'] and results['beginning_counts'][n]['most_common']:
                data = results['beginning_counts'][n]
                top1_pct = (data['most_common'][0][1] / data['total']) * 100
                top3_pct = sum(c[1] for c in data['most_common'][:3]) / data['total'] * 100
                top5_pct = sum(c[1] for c in data['most_common'][:5]) / data['total'] * 100
                
                top1_percentages.append(top1_pct)
                top3_percentages.append(top3_pct)
                top5_percentages.append(top5_pct)
            else:
                top1_percentages.append(0)
                top3_percentages.append(0)
                top5_percentages.append(0)
                
        x = np.arange(len(n_values))
        width = 0.25
        
        bars1 = ax.bar(x - width, top1_percentages, width, label='Top 1', 
                       color=colors[idx], edgecolor='black', linewidth=1)
        bars3 = ax.bar(x, top3_percentages, width, label='Top 3', 
                       color=colors[idx], alpha=0.6, edgecolor='black', linewidth=1)
        bars5 = ax.bar(x + width, top5_percentages, width, label='Top 5', 
                       color=colors[idx], alpha=0.3, edgecolor='black', linewidth=1)
                       
        ax.set_xlabel('Length of Ending (n elements from 1)', fontsize=11)
        ax.set_ylabel('Coverage (%)', fontsize=11)
        ax.set_title(format_title(range_val), fontsize=13, fontweight='bold')
        ax.set_xticks(x)
        ax.set_xticklabels(n_values, fontsize=10)
        ax.legend(fontsize=9, loc='upper right')
        
        max_val = max(top5_percentages) if top5_percentages else 10
        ax.set_ylim(0, max(max_val * 1.25, 10))
        ax.grid(axis='y', alpha=0.3)
        
        # Reduced font size for dense overlapping areas
        for bar, pct in zip(bars1, top1_percentages):
            if pct > 0:
                ax.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.5,
                        f'{pct:.0f}%', ha='center', va='bottom', fontsize=8, fontweight='bold')
                        
    for idx in range(num_ranges, rows * cols):
        axes[idx].set_visible(False)
        
    plt.suptitle(f'{TARGET_ALGO}: Coverage of Top Sequence Endings (from 1){limit_label}', fontsize=16, fontweight='bold')
    plt.tight_layout(rect=[0, 0.02, 1, 0.96])
    plt.savefig(f'{OUTPUT_DIR}/plot6_endings_coverage.png', dpi=150, bbox_inches='tight')
    plt.close()

def export_to_txt(all_results, sorted_ranges, limit_label):
    """Export analysis results to TXT files for each range."""
    for range_val in sorted_ranges:
        results = all_results[range_val]
        txt_path = f'{OUTPUT_DIR}/analysis_range_{range_val}.txt'
        
        with open(txt_path, 'w', encoding='utf-8') as f:
            f.write(f"{'='*60}\n")
            f.write(f"  {TARGET_ALGO} - {format_title(range_val)} Analysis\n")
            if limit_label:
                f.write(f"  {limit_label.strip()}\n")
            f.write(f"{'='*60}\n\n")
            
            f.write(f"1. Number of sequences generated: {results['num_sequences']}\n\n")
            
            f.write(f"2. Population Intervals Between Sequences:\n")
            f.write(f"   Average: {results['avg_interval']:.2f}\n")
            f.write(f"   Median:  {results['med_interval']:.2f}\n")
            f.write(f"   Minimum: {results['min_interval']}\n")
            f.write(f"   Maximum: {results['max_interval']}\n")
            f.write(f"   Std Dev: {results['std_interval']:.2f}\n")
            f.write(f"   (Max Pop Recorded: {max(results['populations']) if results['populations'] else 0})\n\n")
            
            f.write(f"3. Sequence Statistics:\n")
            f.write(f"   Min sequence: {results['min_seq']}\n")
            f.write(f"   Avg sequence: {[round(float(x), 2) for x in results['avg_seq']]}\n")
            f.write(f"   Max sequence: {results['max_seq']}\n\n")
            
            f.write(f"   Position-by-position (Aligned from End):\n")
            f.write(f"   {'Pos':<7} {'Min':<10} {'Avg':<12} {'Max':<10}\n")
            f.write(f"   {'-'*39}\n")
            max_len = len(results['avg_seq'])
            
            # Iterate backwards so the last gap (end of the list) prints first as Pos 1
            for i in range(max_len):
                idx = max_len - 1 - i
                pos_label = str(i + 1)
                f.write(f"   {pos_label:<7} {results['min_seq'][idx]:<10} {results['avg_seq'][idx]:<12.2f} {results['max_seq'][idx]:<10}\n")
                
            f.write(f"\n4. Sequence Endings (from 1):\n")
            for n, data in results['beginning_counts'].items():
                f.write(f"\n   Last {n} element(s) - {data['total']} sequences, {len(data['counts'])} unique patterns:\n")
                for seq, count in data['most_common']:
                    percentage = (count / data['total']) * 100
                    seq_str = ', '.join(map(str, seq))
                    f.write(f"      [{seq_str}]: {count} times ({percentage:.2f}%)\n")
                    
            f.write(f"\n5. Most Common Gaps by Position (from 1):\n")
            for pos in range(1, min(11, max_len + 1)):  # Show up to 10 positions
                if pos in results['gap_freqs_by_pos']:
                    data = results['gap_freqs_by_pos'][pos]
                    f.write(f"\n   Position {pos} (from end) - {data['total']} valid gaps, {len(data['counts'])} unique values:\n")
                    for gap, count in data['most_common'][:5]:  # Top 5 per position
                        percentage = (count / data['total']) * 100
                        f.write(f"      Gap [{gap}]: {count} times ({percentage:.2f}%)\n")
            
            f.write(f"\n6. Sequence Types & Flags:\n")
            f.write(f"   Total Unique Types: {len(results['types_counter'])}\n")
            f.write(f"   ---------------------------------------\n")
            for type_val, count in results['types_counter'].most_common():
                percentage = (count / results['num_sequences']) * 100
                f.write(f"   {type_val:<20} {count:>5} ({percentage:>5.1f}%)\n")
                
            f.write(f"\n   Flags Detected:\n")
            f.write(f"   ---------------------------------------\n")
            mut_pct = (results['mut_count'] / results['num_sequences']) * 100 if results['num_sequences'] else 0
            val_pct = (results['val_count'] / results['num_sequences']) * 100 if results['num_sequences'] else 0
            f.write(f"   Contains |Mutated:   {results['mut_count']:>5} ({mut_pct:>5.1f}%)\n")
            f.write(f"   Contains |Validated: {results['val_count']:>5} ({val_pct:>5.1f}%)\n")

def main():
    if MAX_POPULATIONS is not None and MAX_SEQUENCES is not None:
        raise ValueError("Cannot set both MAX_POPULATIONS and MAX_SEQUENCES at the same time. Please set one of them to None.")

    os.makedirs(OUTPUT_DIR, exist_ok=True)
    
    all_results = {}
    
    for range_val, filename in FILES.items():
        filepath = os.path.join(RESULTS_DIR, filename)
        if not os.path.exists(filepath):
            print(f"Warning: File not found: {filepath}")
            continue
            
        print(f"Processing {format_title(range_val)}...")
        sequences = parse_file(filepath, max_pops=MAX_POPULATIONS, max_seqs=MAX_SEQUENCES)
        results = analyze_sequences(sequences)
        
        if results:
            all_results[range_val] = results
            
    if not all_results:
        print("No valid data to analyze!")
        return
        
    sorted_ranges = get_sorted_ranges(list(FILES.keys()))
    sorted_ranges = [r for r in sorted_ranges if r in all_results]
    
    colors = get_dynamic_colors(len(sorted_ranges))
    
    limit_label = ""
    if MAX_POPULATIONS is not None:
        limit_label = f"\n (Max Iterations: {MAX_POPULATIONS})"
    elif MAX_SEQUENCES is not None:
        limit_label = f"\n (Max Sequences: {MAX_SEQUENCES})"
    
    print("\n" + "="*60)
    print("  EXPORTING TXT FILES")
    print("="*60)
    export_to_txt(all_results, sorted_ranges, limit_label)
    
    print("\n" + "="*60)
    print("  GENERATING PLOTS")
    print("="*60)
    
    plot_num_sequences(all_results, sorted_ranges, colors, limit_label)
    plot_avg_interval(all_results, sorted_ranges, colors, limit_label)
    plot_common_beginnings(all_results, sorted_ranges, colors, limit_label)
    plot_common_gaps_by_pos(all_results, sorted_ranges, colors, limit_label)
    plot_common_beginnings_detailed(all_results, sorted_ranges, colors, limit_label)
    
    print(f"\nAnalysis complete! Results saved to '{OUTPUT_DIR}/'")

if __name__ == "__main__":
    main()