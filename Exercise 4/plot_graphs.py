"""IT24103447 - Plot actual MPI timings using Python and Matplotlib."""

import statistics

import matplotlib.pyplot as plt


PROGRAMS = {"Summation": "#1463ab", "Monte Carlo Pi": "#d96b18"}
LABELS = {"Summation": "Exercise 2 - Summation", "Monte Carlo Pi": "Exercise 3 - Monte Carlo Pi"}

# IT24103447 - Actual MPI_Wtime measurements in seconds, five runs per setting.
# IT24103447 - Each run used 10,000,000 values/trials. Warm-ups are excluded.
# IT24103447 - One process used one EC2 node; 2/4 processes used two EC2 nodes.
# IT24103447 - The sheet's ambiguous numbering is interpreted as Exercises 2 and 3.
TIMINGS = {
    "Summation": {
        1: [0.025989522, 0.025638060, 0.026689054, 0.025270407, 0.025506443],
        2: [0.013015323, 0.013311202, 0.013256213, 0.012851971, 0.013211654],
        4: [0.009736226, 0.009715134, 0.017435152, 0.010402653, 0.009762213],
    },
    "Monte Carlo Pi": {
        1: [0.350713985, 0.349843382, 0.349706322, 0.349774468, 0.350384523],
        2: [0.175455369, 0.175992522, 0.175748885, 0.176260217, 0.175425606],
        4: [0.152269081, 0.142322873, 0.142199524, 0.160613696, 0.142511251],
    },
}


def main():
    summaries = []
    for program in PROGRAMS:
        baseline = None
        for count in (1, 2, 4):
            times = TIMINGS[program][count]
            if len(times) != 5 or any(time <= 0 for time in times):
                raise ValueError(f"Expected five positive timings: {program}, {count}")
            median = statistics.median(times)
            if count == 1:
                baseline = median
            summaries.append({"program": program, "processes": count,
                              "nodes": 1 if count == 1 else 2, "runs": len(times),
                              "median_seconds": median, "min_seconds": min(times),
                              "max_seconds": max(times), "speedup": baseline / median})

    plt.rcParams.update({"font.size": 11, "axes.spines.top": False,
                         "axes.spines.right": False, "figure.facecolor": "white"})
    for metric, title, ylabel in [
        ("median_seconds", "Execution time vs number of MPI processes",
         "Execution time (seconds)"),
        ("speedup", "Speedup vs number of MPI processes",
         "Speedup (T1 / Tp)"),
    ]:
        fig, ax = plt.subplots(figsize=(9, 6))
        fig.subplots_adjust(left=0.12, right=0.97, bottom=0.22, top=0.84)
        for program, color in PROGRAMS.items():
            points = [row for row in summaries if row["program"] == program]
            values = [row[metric] for row in points]
            ax.plot([1, 2, 4], values, marker="o", linewidth=2.2,
                    markersize=7, color=color, label=LABELS[program])
            for count, value in zip((1, 2, 4), values):
                if metric == "speedup" and count == 1 and program == "Monte Carlo Pi":
                    continue
                offset = (0, 10)
                if metric == "speedup" and count > 1:
                    offset = (0, -19) if program == "Monte Carlo Pi" else (0, 12)
                ax.annotate(f"{value:.4f}" if metric == "median_seconds" else f"{value:.2f}x",
                            (count, value), xytext=offset, textcoords="offset points",
                            ha="center", fontsize=9, color=color)
        if metric == "speedup":
            ax.plot([1, 2, 4], [1, 2, 4], "--", color="#78818c", label="Ideal speedup")
        ax.set(title=title, xlabel="Number of processors (MPI processes)", ylabel=ylabel,
               xticks=[1, 2, 4], xlim=(0.7, 4.3))
        ax.set_ylim(bottom=0)
        ax.margins(y=0.2)
        ax.grid(axis="y", alpha=0.2)
        ax.legend(loc="best")
        fig.suptitle("Exercise 4 - IT24103447", y=0.98, fontsize=10, color="#5a626b")
        fig.text(0.5, 0.035,
                 "Median of 5 runs; 10,000,000 values/trials per run.\n"
                 "1 process: one EC2 node. 2/4 processes: two EC2 nodes; no oversubscription.",
                 ha="center", fontsize=9, color="#5a626b")
    for row in summaries:
        print(f"{row['program']}: p={row['processes']}, "
              f"median={row['median_seconds']:.9f}s, speedup={row['speedup']:.3f}")

    # IT24103447 - Display both graphs in windows without saving image files.
    plt.show()


if __name__ == "__main__":
    main()
