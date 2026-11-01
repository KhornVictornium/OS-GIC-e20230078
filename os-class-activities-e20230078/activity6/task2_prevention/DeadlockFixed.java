
import java.util.concurrent.Semaphore;

class BankAccount {
    String name;
    int balance;

    BankAccount(String name, int balance) {
        this.name = name;
        this.balance = balance;
    }
}

class SafeTransfer {

    // One shared mutex for ALL bank transfers.
    static Semaphore mutex = new Semaphore(1);

    static void transfer(
            BankAccount from,
            BankAccount to,
            int amount) {

        boolean locked = false;

        try {
            System.out.println(
                Thread.currentThread().getName()
                + " waiting for banking mutex..."
            );

            // Acquire the shared mutex.
            mutex.acquire();
            locked = true;

            System.out.println(
                Thread.currentThread().getName()
                + " acquired banking mutex."
            );

            // Simulate processing time.
            Thread.sleep(500);

            // Perform the transfer.
            from.balance -= amount;
            to.balance += amount;

            System.out.println(
                Thread.currentThread().getName()
                + " transferred " + amount
                + " from " + from.name
                + " to " + to.name
            );

            System.out.println(
                "Current balances: "
                + from.name + " = " + from.balance
                + ", " + to.name + " = " + to.balance
            );

        } catch (InterruptedException e) {
            System.out.println("Transfer interrupted.");
            Thread.currentThread().interrupt();

        } finally {
            // Always release the mutex if acquired.
            if (locked) {
                mutex.release();

                System.out.println(
                    Thread.currentThread().getName()
                    + " released banking mutex."
                );
            }
        }
    }
}

public class DeadlockFixed {

    public static void main(String[] args)
            throws InterruptedException {

        BankAccount accountA =
                new BankAccount("Account A", 1000);

        BankAccount accountB =
                new BankAccount("Account B", 1000);

        // Record the original total.
        int startingTotal =
                accountA.balance + accountB.balance;

        System.out.println("===== DEADLOCK PREVENTION =====");

        System.out.println("Starting Account A: " + accountA.balance);
        System.out.println("Starting Account B: " + accountB.balance);
        System.out.println("Starting Total: " + startingTotal);

        // Transaction 1: A -> B, amount 100.
        Thread worker1 = new Thread(
            () -> SafeTransfer.transfer(
                accountA, accountB, 100
            ),
            "Worker 1"
        );

        // Transaction 2: B -> A, amount 200.
        Thread worker2 = new Thread(
            () -> SafeTransfer.transfer(
                accountB, accountA, 200
            ),
            "Worker 2"
        );

        // Start both workers.
        worker1.start();
        worker2.start();

        // Wait until both transfers finish.
        worker1.join();
        worker2.join();

        // Print final balances.
        int finalTotal =
                accountA.balance + accountB.balance;

        System.out.println();
        System.out.println("===== FINAL RESULTS =====");

        System.out.println("Final Account A: " + accountA.balance);
        System.out.println("Final Account B: " + accountB.balance);
        System.out.println("Final Total: " + finalTotal);

        System.out.println("Starting Total: " + startingTotal);

        if (startingTotal == finalTotal) {
            System.out.println("Balance verification: PASSED");
        } else {
            System.out.println("Balance verification: FAILED");
        }

        System.out.println("No deadlock occurred");
    }
}

