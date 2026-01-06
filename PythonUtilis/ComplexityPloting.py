import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter
from pathlib import Path

# Get the directory where this script is located
script_dir = Path(__file__).parent
output_dir = script_dir / "outputs"

# Custom formatter to display full scientific notation on each tick
def sci_notation(x, pos):
    if x == 0:
        return '0'
    if abs(x) >= 1e4 or abs(x) < 1e-3:
        return f'{x:.1e}'
    return f'{x:.0f}'

# Create outputs directory if it doesn't exist
output_dir.mkdir(exist_ok=True)

# Plot 1: Small n (up to 100)
n_small = np.linspace(1, 100, 1000)

# Calculate complexity functions for small n
n_squared_small = n_small**2
n_3_2_small = n_small**(3/2)
n_4_3_small = n_small**(4/3)
n_log_n_small = n_small * np.log2(n_small)
n_log2_n_small = n_small * (np.log2(n_small)**2)

# Create the plot for small n
plt.figure(figsize=(10, 6))
plt.plot(n_small, n_3_2_small, label='$n^{3/2}$', linewidth=2)
plt.plot(n_small, n_4_3_small, label='$n^{4/3}$', linewidth=2)
plt.plot(n_small, n_log_n_small, label='$n \\log n$', linewidth=2)
plt.plot(n_small, n_log2_n_small, label='$n \\log^2 n$', linewidth=2)

# Get current y-axis limits before adding n^2
ylim = plt.ylim()

# Add n^2 without affecting the scale
plt.plot(n_small, n_squared_small, label='$n^2$', linewidth=2, linestyle='--')

# Restore the y-axis limits
plt.ylim(ylim)

plt.xlabel('n', fontsize=12)
plt.ylabel('Operations', fontsize=12)
plt.gca().yaxis.set_major_formatter(FuncFormatter(sci_notation))
plt.title('Time Complexity Growth (n 1-100)', fontsize=14)
plt.legend(fontsize=11, loc='upper left')
plt.grid(True, alpha=0.3)

# Save the plot for small n
output_path_small = output_dir / "time_complexity_growth_small.png"
plt.savefig(output_path_small, dpi=300, bbox_inches='tight')
print(f"Small n plot saved to: {output_path_small}")
plt.close()

# Plot 2: Large n (up to 100000)
n_large = np.linspace(1, 100000, 1000)

# Calculate complexity functions for large n
n_squared_large = n_large**2
n_3_2_large = n_large**(3/2)
n_4_3_large = n_large**(4/3)
n_log_n_large = n_large * np.log2(n_large)
n_log2_n_large = n_large * (np.log2(n_large)**2)

# Create the plot for large n
plt.figure(figsize=(10, 6))
plt.plot(n_large, n_3_2_large, label='$n^{3/2}$', linewidth=2)
plt.plot(n_large, n_4_3_large, label='$n^{4/3}$', linewidth=2)
plt.plot(n_large, n_log_n_large, label='$n \\log n$', linewidth=2)
plt.plot(n_large, n_log2_n_large, label='$n \\log^2 n$', linewidth=2)

# Get current y-axis limits before adding n^2
ylim = plt.ylim()

# Add n^2 without affecting the scale
plt.plot(n_large, n_squared_large, label='$n^2$', linewidth=2, linestyle='--')

# Restore the y-axis limits
plt.ylim(ylim)

plt.xlabel('n', fontsize=12)
plt.ylabel('Operations', fontsize=12)
plt.gca().yaxis.set_major_formatter(FuncFormatter(sci_notation))
plt.title('Time Complexity Growth (n 1-100000)', fontsize=14)
plt.legend(fontsize=11, loc='upper left')
plt.grid(True, alpha=0.3)

# Save the plot for large n
output_path_large = output_dir / "time_complexity_growth_large.png"
plt.savefig(output_path_large, dpi=300, bbox_inches='tight')
print(f"Large n plot saved to: {output_path_large}")
plt.close()
