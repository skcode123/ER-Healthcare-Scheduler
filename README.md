**Algorithm Description**

This program implements a **greedy priority-based scheduling algorithm** to assign patients to doctors in an emergency room. The objective is to minimize the **total risk**, calculated as the product of patient severity and waiting time before treatment.
Patient data is read from a CSV file and stored in an array. Each patient record contains the patient ID, severity, arrival time, treatment duration, and required specialization. The system includes three doctors: Trauma, Cardio, and General. The General doctor can treat patients of any specialization.
The scheduling process follows an **event-driven simulation**. The algorithm repeatedly checks for available doctors and selects the best untreated patient who has already arrived and is compatible with the doctor’s specialization. A **priority score** is calculated for each eligible patient:
Priority = (severity × (waiting_time + 1)) / treatment_time
The patient with the highest score is selected for treatment. This prioritizes patients with higher severity, longer waiting times, and shorter treatment durations. Once selected, the treatment start time is determined, the doctor's availability is updated, and the assignment is recorded in a schedule list.
Instead of increasing time step-by-step, the simulation advances directly to the next event, which is either a doctor becoming free or a new patient arriving. This event-based progression improves efficiency.
After all patients are treated, the program calculates the **total estimated risk** and writes the treatment schedule and risk value to a JSON output file.


**Complexity Analysis**

Let N be the number of patients. For each doctor assignment, the algorithm scans all patients to compute priority scores and select the best candidate. Since each patient is scheduled once and the search checks all patients, the overall time complexity is O(N²).
The space complexity is **O(N)** because patient data and treatment schedules are stored in arrays proportional to the number of patients.

Key Design Decisions
The implementation uses a **dynamic priority function** to balance urgency, fairness, and efficiency. A **specialization compatibility check** ensures patients are assigned only to appropriate doctors, while the General doctor provides flexibility by handling all specializations. Finally, **event-driven time advancement** avoids unnecessary iterations and improves scheduling performance.
