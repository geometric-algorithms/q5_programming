# GeometrixAlgoProject

## Line Segment Intersection

This project deals with computing intersection points among a set of line segments in the plane.

---

### (a) Static Setting: Compute All Intersections

**Problem:**  
Given a set `S` of `n` line segments in the plane.

**Input:**  
A set of `n` line segments.

**Output:**  
The set of all intersection points between segments in `S`.

**Time Complexity:**  
`O(n log n + k)`, where `k` is the number of intersections.

---

### (b) Dynamic Setting: Support for Insertions and Deletions

In the dynamic version of the problem, segments can be added to or removed from the set at any time.

**Operations Supported:**
- **Insert** a segment `ℓᵢ`
- **Delete** a segment `ℓᵢ`
- **Report** the current set of all intersection points

**Output:**  
After each update, the algorithm maintains and reports the current set of intersection points among all active segments.

---

This project involves implementing efficient algorithms for both static and dynamic segment intersection handling.


## Running Instructions
### Part A: Modify input.txt as per need and run ./test.sh 
### Part B: Modify input.txt as per need and run ./test.sh

## Contributors
Karthikeya Satti (210050142), Hrushikesh Reddy M (210050097)
