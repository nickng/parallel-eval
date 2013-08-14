using System;
using System.Diagnostics;
using System.Threading;
using System.Reflection;

/* Class Declaration */
public class DenseLinearAlgebra
{

    /* Declaration of rows, columns and the
     arrays A, b, c that will take place in 
     the computation*/
    private int rows;
    private int columns;
    private int[,] A;
    private int[] b;
    private int[] c;

    /* The number of workers */
    private int workers;

    /* Constructor */
    public DenseLinearAlgebra(int nrows, int ncols, int workers)
    {
        ROWS = nrows;
        COLUMNS = ncols;
        WORKERS = workers;

        A = new int[rows , columns];
        b = new int[columns];
        c = new int[rows];
    }

    /* Initialize array A with random values */
    public void array_initialization()
    {
        Random random = new Random();
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < columns; j++)
                A[i , j] = random.Next(10);
    }

    /* Initialize vector c with random values */
    public void vector_initialize()
    {
        Random random = new Random();
        for (int i = 0; i < b.Length; i++)
            b[i] = random.Next(10);
    }

    /* Fill array c with zeros */
    public void fill_with_zeros()
    {
        for (int i = 0; i < c.Length; i++)
            c[i] = 0;
    }

    /* Main computation */
    public void multiplication(int worker_id)
    {

        /* obtain reference to currently executing thread */
        Thread current = Thread.CurrentThread;

        /* Number of rounds for the computation */
        int rounds = 500;

        /* start and end of the work of each thread */
        int start = (int)Math.Floor((double)(rows * worker_id / workers));
        int end = (int)Math.Floor((double)(rows * (worker_id + 1) / workers));

        Console.WriteLine("Start of work is " + start);
        Console.WriteLine("End of work is " + end);

        while (rounds-- > 0)
        {
            /* Multiplication of A and b */
            for (int i = start; i < end; i++)
                for (int j = 0; j < columns; j++)
                    c[i] += A[i, j] * b[j];
        }//End while

        Console.WriteLine(current.Name + " finished");

    }

    /* Property WORKERS */
    public int WORKERS
    {

        set
        {
            workers = value;
        }

        get
        {
            return workers;
        }

    }

    /* Property ROWS */
    public int ROWS
    {

        set
        {
            rows = value;
        }

        get
        {
            return rows;
        }

    }

    /* Property COLUMNS */
    public int COLUMNS
    {

        set
        {
            columns = value;
        }

        get
        {
            return columns;
        }

    }

    public static void Main()
    {

        /* Create object */
        DenseLinearAlgebra dense = new DenseLinearAlgebra(1000, 1000, 4);

        /* Fill tha arrays with inital values */
        dense.array_initialization();
        dense.vector_initialize();
        dense.fill_with_zeros();

        /* Create the workers */
        Thread worker1 = new Thread(new ThreadStart(() => dense.multiplication(0)));
        worker1.Name = "worker1";

        /* Create the workers */
        Thread worker2 = new Thread(new ThreadStart(() => dense.multiplication(1)));
        worker2.Name = "worker2";

        /* Create the workers */
        Thread worker3 = new Thread(new ThreadStart(() => dense.multiplication(2)));
        worker3.Name = "worker3";

        /* Create the workers */
        Thread worker4 = new Thread(new ThreadStart(() => dense.multiplication(3)));
        worker4.Name = "worker4";

        /* Count the time that has elapsed */
        Stopwatch stopWatch = new Stopwatch();
        stopWatch.Start();


        /* Threads start their work */
        worker1.Start();
        worker2.Start();
        worker3.Start();
        worker4.Start();

        /* Threads have finished their work */
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
    }


}//End of class DenseLinearAlgebra