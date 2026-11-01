# Class Activity 6 - Deadlock Simulation

- **Student Name:** KHORN VICTOR
- **Student ID:** e20230078
- **Programming Language Used:** Java

---

## Task 1: Deadlock Version

![Deadlock version](screenshots/task1_deadlock.png)

- **Shared resources:** `Account A` and `Account B` (their balances and lock semaphores).
- **Transaction 1:** Worker 1 transfers 100 from Account A to Account B.
- **Transaction 2:** Worker 2 transfers 200 from Account B to Account A.
- **Deadlock message shown:** `Deadlock detected: transactions are stuck`
- **Explanation of why the program got stuck:**  
  Worker 1 locked Account A and waited for Account B, while Worker 2 locked Account B and waited for Account A. Since each thread held one lock and waited for the other, neither could proceed, freezing the simulation in deadlock.

---

## Task 2: Deadlock Prevention Version

![Deadlock prevention](screenshots/task2_prevention.png)

- **Prevention strategy used:** A single shared semaphore mutex (`SafeTransfer.mutex`) serializes the entire bank transfer operation.
- **Semaphore mutex initial value:** 1
- **Starting total:** 2000
- **Final total:** 2000
- **Did both transfers complete?** Yes, both Worker 1 and Worker 2 completed their transactions.
- **Why no deadlock occurred:**  
  Both workers must acquire the same shared mutex before accessing any account. Only one worker can perform a transfer at a time, completely removing lock contention between accounts.

---

## Questions

1. **What are the two shared resources in your bank transaction simulation?**  
   > The two shared resources are `Account A` and `Account B`, which both worker threads attempt to access and update.

2. **Which line or section of your Task 1 program creates hold-and-wait?**  
   > In `Transfer.java` (lines 31–47), a thread acquires `from.lock.acquire()`, holds it during `Thread.sleep(500)`, and then attempts to acquire `to.lock.acquire()`. Holding the source account lock while waiting for the destination account lock creates hold-and-wait.

3. **How does Task 1 create circular wait?**  
   > Worker 1 locks Account A and waits for Account B, while Worker 2 locks Account B and waits for Account A. This forms a closed loop of dependencies (Worker 1 $\rightarrow$ Account B $\rightarrow$ Worker 2 $\rightarrow$ Account A $\rightarrow$ Worker 1).

4. **Why does the Task 1 program need a watchdog or timeout?**  
   > Without a timeout, calling `join()` would freeze the main program forever because deadlocked threads never finish. The watchdog (`worker1.join(3000)`) gives the threads 3 seconds to complete before waking up to detect the deadlock and report diagnostics.

5. **How does the single semaphore mutex prevent deadlock in Task 2?**  
   > The single mutex ensures that only one transfer can execute at any time. A worker finishes its entire transfer and releases the mutex before another worker can start, eliminating overlapping locks.

6. **Which of the four deadlock conditions does your Task 2 solution remove or avoid?**  
   > It eliminates **Hold-and-Wait** and **Circular Wait**. Because threads acquire only a single global lock rather than holding one account lock while waiting for another, no cycle or incremental lock holding can occur.

7. **Why must the final total bank balance remain unchanged after both transfers?**  
   > Transfers only move money between accounts without adding or subtracting funds. Preserving the total balance (2000) verifies transactional consistency and proves no race condition or partial transfer occurred.

---

## Reflection

> This activity demonstrated that deadlocks occur when concurrent processes acquire multiple shared resources in conflicting order. In real-world systems like banking databases and file systems, deadlocks can freeze critical operations unless prevented through techniques like global locking, ordered resource acquisition, or timeout-based rollbacks. It highlighted the importance of designing synchronization carefully so that data consistency is preserved without causing threads to block each other indefinitely.
