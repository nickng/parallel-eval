using System;
using System.Diagnostics;
using System.Threading;

/* Class monte Carlo */
public class MonteCarlo
{
    /* The final pi and the number of workers */
    private double final_pi;
    private int num_of_workers;
    private int bullets;

    /* Constructor */
    public MonteCarlo(int workers, int bullet)
    {
        this.num_of_workers = workers;
        BULLETS = bullet;
        PI = 0.0;
    }

    /* Property PI */
    public double PI
    {
        get
        {
            return final_pi;
        }

        set
        {
            final_pi = value;
        }
    }

    /* Property BULLETS */
    public int BULLETS
    {
        get
        {
            return bullets;
        }

        set
        {
            bullets = value;
        }
    }

    /* Print the final pi */
    public void print_pi()
    {
        /* Divide with the num of workers */
        PI = PI / (double)num_of_workers;

        /* Print the Pi */
        Console.WriteLine();
        Console.WriteLine("Final pi is : " + PI);
    }

    /* Worker thread computes the pi
    * for a number of bullets*/
    public void worker(int worker_id)
    {
        /* Obtain reference to currently executing thread */
        Thread current = Thread.CurrentThread;

        /* Define the work that each thread will perform */
        int start = (int)Math.Floor((double)(bullets * worker_id / num_of_workers));
        int end = (int)Math.Floor((double)(bullets * (worker_id + 1) / num_of_workers));

        /* Run the main computation for a specific number of rounds */
        int rounds = 0;

        /* Variables declaration */
        double current_pi = 0.0;
        double x;
        double y;
        int i;
        int inCircle = 0;

        /* Random seed */
        Random r = new Random(worker_id + 5);

        while (rounds++ < 10)
        {
            /* For all points compute the  */
            for (i = start; i < end; i++)
            {
                x = r.NextDouble() * 2;
                y = r.NextDouble() * 2;

                if (x * x + y * y < 1.0)
                    inCircle++;
            }

            /* Compute the pi */
            current_pi = (worker_id + 1) * 16 * (double)inCircle / (double)i;
        }

        /* Final pi for this workers */
        current_pi = current_pi / (rounds - 1);

        /* get the final pi */
        PI += current_pi;

        Console.WriteLine("Pi in worker " + worker_id + " is " + current_pi);
        Console.WriteLine(current.Name + " finished");
    }

    /* Main function */
    public static void Main()
    {

        /* Create object mcarlo */
        MonteCarlo mcarlo = new MonteCarlo(4, 40000000);

        /* Count the time that has elapsed */
        Stopwatch stopWatch = new Stopwatch();
        stopWatch.Start();

        /* Create the threads */
        Thread worker1 = new Thread(new ThreadStart(()=> mcarlo.worker(0)));
        worker1.Name = "worker1";
        
        Thread worker2 = new Thread(new ThreadStart(() => mcarlo.worker(1)));
        worker2.Name = "worker2";

        Thread worker3 = new Thread(new ThreadStart(() => mcarlo.worker(2)));
        worker3.Name = "worker3";

        Thread worker4 = new Thread(new ThreadStart(() => mcarlo.worker(3)));
        worker4.Name = "worker4";
        
        /* Threads start their work */
        worker1.Start();
        worker2.Start();
        worker3.Start();
        worker4.Start();
        
        /* Threads finish their work */
        worker1.Join();
        worker2.Join();
        worker3.Join();
        worker4.Join();
        
        /* Stop the clock */
        stopWatch.Stop();
        /* Get the elapsed time as a TimeSpan value. */
        TimeSpan ts = stopWatch.Elapsed;

        Console.WriteLine();
        Console.WriteLine("Elapsed time:");
        /* Format and display the TimeSpan value. */
        string elapsedTime = String.Format("{0:00}:{1:00}:{2:00}.{3:00}",
            ts.Hours, ts.Minutes, ts.Seconds,
            ts.Milliseconds / 10);
        Console.WriteLine(elapsedTime, "RunTime");

        /* Print the final pi */
        mcarlo.print_pi();

    }

}//End class Monte Carlo

