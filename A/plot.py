import matplotlib.pyplot as plt

def read_segments(file_path):
    segments = []
    with open(file_path, 'r') as f:
        lines = f.read().splitlines()
        n = int(lines[0])
        for line in lines[1:n+1]:
            x1, y1, x2, y2 = map(float, line.strip().split())
            segments.append(((x1, y1), (x2, y2)))
    return segments

def read_points(file_path):
    points = []
    with open(file_path, 'r') as f:
        for line in f:
            if line.strip():
                x, y = map(float, line.strip().split())
                points.append((x, y))
    return points

def plot(segments, points):
    fig, ax = plt.subplots()

    endpoints = []

    # Plot segments and IDs
    for idx, (p1, p2) in enumerate(segments):
        x_vals = [p1[0], p2[0]]
        y_vals = [p1[1], p2[1]]
        ax.plot(x_vals, y_vals, 'b-')

        # Store endpoints
        endpoints.extend([p1, p2])

        # Calculate and annotate midpoint with segment ID
        mid_x = (p1[0] + p2[0]) / 2
        mid_y = (p1[1] + p2[1]) / 2
        ax.annotate(f'ID {idx}', xy=(mid_x, mid_y), xytext=(0, 5),
                    textcoords='offset points', fontsize=8, color='black', ha='center')

    # Plot segment endpoints with coordinates
    if endpoints:
        exs, eys = zip(*endpoints)
        ax.scatter(exs, eys, color='green', s=30, label='Segment Endpoints', zorder=3)
        for x, y in endpoints:
            ax.annotate(f'({x:.2f}, {y:.2f})', xy=(x, y), xytext=(5, 5),
                        textcoords='offset points', fontsize=8, color='darkgreen')

    # Plot intersection points with coordinates
    if points:
        xs, ys = zip(*points)
        ax.scatter(xs, ys, color='red', s=50, zorder=5, label='Intersections')
        for x, y in points:
            ax.annotate(f'({x:.2f}, {y:.2f})', xy=(x, y), xytext=(5, 5),
                        textcoords='offset points', fontsize=8, color='darkred')

    ax.set_aspect('equal')
    ax.grid(True)
    ax.legend()
    plt.title("Line Segments with IDs, Endpoints, and Intersections")
    plt.xlabel("x")
    plt.ylabel("y")
    plt.show()

if __name__ == "__main__":
    segments = read_segments("in.txt")
    points = read_points("out.txt")
    plot(segments, points)
