
import java.util.concurrent.Semaphore;
import java.util.concurrent.atomic.AtomicInteger;

class Account {
    String name;
    int balance;

    Semaphore lock = new Semaphore(1);

    Account(String name, int balance) {
        this.name = name;
        this.balance = balance;
    }
}

class Transfer {

    static AtomicInteger completedTransfers = new AtomicInteger(0);

    static void transfer(Account from, Account to, int amount) {
        boolean fromLocked = false;
        boolean toLocked = false;

        try {
            System.out.println(
                Thread.currentThread().getName()
                + " trying to lock FROM " + from.name
            );

            from.lock.acquire();
            fromLocked = true;

            System.out.println(
                Thread.currentThread().getName()
                + " locked FROM " + from.name
            );

            // Give the other thread time to lock its source account.
            Thread.sleep(500);

            System.out.println(
                Thread.currentThread().getName()
                + " waiting for TO " + to.name
            );

            to.lock.acquire();
            toLocked = true;

            System.out.println(
                Thread.currentThread().getName()
                + " locked TO " + to.name
            );

            from.balance -= amount;
            to.balance += amount;

            completedTransfers.incrementAndGet();

            System.out.println(
                Thread.currentThread().getName()
                + " transferred " + amount
                + " from " + from.name
                + " to " + to.name
            );

        } catch (InterruptedException e) {
            System.out.println(
                Thread.currentThread().getName() + " interrupted."
            );
            Thread.currentThread().interrupt();

        } finally {
            if (toLocked) {
                to.lock.release();
            }

            if (fromLocked) {
                from.lock.release();
            }
        }
    }
}

public class DeadlockSimulation {

    public static void main(String[] args) throws InterruptedException {

        Account accountA = new Account("Account A", 1000);
        Account accountB = new Account("Account B", 1000);

        System.out.println("===== BANK DEADLOCK SIMULATION =====");
        System.out.println("Starting Account A: " + accountA.balance);
        System.out.println("Starting Account B: " + accountB.balance);
        System.out.println(
            "Starting Total: " +
            (accountA.balance + accountB.balance)
        );

        Thread worker1 = new Thread(
            () -> Transfer.transfer(accountA, accountB, 100),
            "Worker 1"
        );

        Thread worker2 = new Thread(
            () -> Transfer.transfer(accountB, accountA, 200),
            "Worker 2"
        );

        worker1.setDaemon(true);
        worker2.setDaemon(true);

        worker1.start();
        worker2.start();

        // Watchdog: wait up to 3 seconds for a transfer to complete.
        worker1.join(3000);

        if (Transfer.completedTransfers.get() == 0) {
            System.out.println();
            System.out.println(
                "Deadlock detected: transactions are stuck"
            );
            System.out.println("Current lock situation:");
            System.out.println("Worker 1 holds Account A, waiting for Account B");
            System.out.println("Worker 2 holds Account B, waiting for Account A");

            System.out.println("Worker 1 state: " + worker1.getState());
            System.out.println("Worker 2 state: " + worker2.getState());

            System.out.println("Account A balance: " + accountA.balance);
            System.out.println("Account B balance: " + accountB.balance);
        } else {
            System.out.println("A transfer completed. No deadlock detected.");
        }
    }
}

