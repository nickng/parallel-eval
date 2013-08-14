using System;
using System.Diagnostics;
using System.Threading;
using System.Reflection;


/* Create class for the Sparse Linear Algebra dwarf */
public class SparseLinearAlgebra
{

    /* Size of vectors and arrays */
    private int rows;
    private int columns;

    /* Declaration of vectors and arrays */
    private int[,] A;
    private int []results;
    private int[] x;

    /* The array that contains the start of each row within
    the non-zero elements array*/
    private int[] row_ptr;

    /* The array that holds the non-zero values */
    private int[] values;

    /* Declaration of array that contains the
     * column number associated with each non zero element*/
    private int[] col_ind;

    /* Variable that holds the non zero elements of the array */
    private int non_zero_elements;

    /* Number of workers */
    private int workers;

    /* Constructor */
    public SparseLinearAlgebra(int nrows, int ncols, int workers)
    {

        ROWS = nrows;
        COLUMNS = ncols;
        this.workers = workers;
        Console.WriteLine("Num of rows are : " + rows);
        Console.WriteLine("Num of columns are : " + columns);
        Console.WriteLine("Num of workers are : " + workers);
        A = new int[rows,columns];
        x = new int[columns];
        results = new int[rows];
        
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

    /* Property NON_ZEROS */
    public int NON_ZEROS
    {

        set
        {
            non_zero_elements = value;
        }

        get
        {
            return non_zero_elements;
        }

    }

    /* Print the array */
    public void print_array()
    {
        Console.WriteLine("Printig the array...");
        for(int i = 0; i < rows; i++){
            for(int j = 0; j < columns; j++){
                Console.Write(A[i, j] + "  ");
            }
            Console.WriteLine();
        }
    }

    /* Print vectors */
    public void print_vectors()
    {
        /* Print vector values */
        Console.WriteLine("Printing values array:");
        for (int i = 0; i < values.Length; i++)
            Console.Write(values[i] + " ");

        Console.WriteLine("Printing col_ind array:");
        for (int i = 0; i < col_ind.Length; i++)
            Console.Write(col_ind[i] + "  ");
    }

    /* Initialize arrays and vectors with random values */
    public void initalization()
    {

        /* Random number generator */
        Random random = new Random();

        /* Flag that shows if we have a zero element on a row */
        int non_zero_flag = 0;

        /* Array A */
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < columns; j++)
            {
                A[i, j] = random.Next(2);
                //Console.Write(A[i, j] + "  ");
                if (A[i, j] != 0) non_zero_flag = 1;
                if (non_zero_flag == 0)
                {
                    A[i, 0] = 1;
                    non_zero_flag = 0;
                }
            }
        }

        /* Vector x */
        for (int i = 0; i < x.Length; i++)
            x[i] = random.Next(10);

        /* Array that will hold the results */
        for (int i = 0; i < results.Length; i++)
            results[i] = 0;
        Console.Write("length = " + results.Length + "  ");
        Console.WriteLine("Initialization of arrays have been completed.");


        /* the non zero elements */
        non_zero_elements = num_of_non_zero_elements();
        Console.WriteLine("Non zero : " + non_zero_elements);

        /* Allocate memory for values array */
        values = new int[non_zero_elements];
        col_ind = new int[non_zero_elements];
        row_ptr = new int[rows + 1];

    }

    /* Fill the arrays with values */
    public void fill_arrays()
    {
        /* Variable for values vector */
        int counter = 0;

        /* Variables for col_ind vector */
        int cols = 0;
        int index = 0;

        /* Fill values array */
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < columns; j++)
                if (A[i, j] != 0)
                    values[counter++] = A[i,j];

         /* Fill values array */
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < columns; j++)
            {
                if (A[i, j] != 0)
                    col_ind[index++] = cols;
                cols++;
            }
            /* Make cols 0 after each iteration */
            cols = 0;
        }
    }

    /* Fill row_ptr array */
    public void fill_row_ptr()
    {
        /* Variables declaration that help with filling the row_ptr array */
        int new_row = 0;
        int no_zero = 0;
        int row_index = 0;

        for(int i = 0 ; i < rows; i++){
            new_row = 1;
            for(int j = 0; j < columns; j++){
                if(A[i,j] != 0){
                    if(new_row == 1){
                        row_ptr[row_index++] = no_zero;
                        new_row = 0;
                    }
                    no_zero++;
                }
            }
        }

        row_ptr[rows] = no_zero; 

    }

    /* Print row_ptr array */
    public void print_row_ptr()
    {
        Console.WriteLine();
        Console.WriteLine("Printing row ptr : ");

        for (int i = 0; i < rows + 1; i++)
            Console.WriteLine("row = " + row_ptr[i]);
    }

    /* Function that counts the number of non zero elements */
    public int num_of_non_zero_elements()
    {

        /* counter variable that holds the number of non zeros */
        int counter = 0;

        for (int i = 0; i < rows; i++)
            for (int j = 0; j < columns; j++)
                if (A[i, j] != 0)
                    counter++;

        /* Return the number found */
        return counter;

    }

    /* Main computation */
    public void computation(int worker_id)
    {
        /* obtain reference to currently executing thread */
        Thread current = Thread.CurrentThread;

        /* Useful variables */
        int i = 0;
        int j = 0;

        /* number of rounds to run the algorithm */
        int rounds = 1000;

        /* The start and end of work for each thread */
        int start = (int)Math.Floor((double)(rows * worker_id / workers));
        int end = (int)Math.Floor((double)(rows * (worker_id + 1) / workers));

        Console.WriteLine("Start of works is : " + start);
        Console.WriteLine("End of works is : " + end);

        /* For 1000 rounds */
        while (rounds-- > 0)
        {
            for (i = start; i < end; i++)
                for (j = row_ptr[i]; j < row_ptr[i + 1]; j++)
                    results[i] += values[j] * x[col_ind[j]];
        }

        Console.WriteLine(current.Name + " finished");
    }


    public static void Main()
    {

        /* Create object */
        SparseLinearAlgebra sparse = new SparseLinearAlgebra(1000, 1000, 4);
        sparse.initalization();
        //sparse.print_array();
        sparse.fill_arrays();
        //sparse.print_vectors();

        sparse.fill_row_ptr();
        //sparse.print_row_ptr();

        /* Create the workers */
        Thread worker1 = new Thread(new ThreadStart(() => sparse.computation(0)));
        worker1.Name = "worker1";

        Thread worker2 = new Thread(new ThreadStart(() => sparse.computation(1)));
        worker2.Name = "worker2";

        Thread worker3 = new Thread(new ThreadStart(() => sparse.computation(2)));
        worker3.Name = "worker3";

        Thread worker4 = new Thread(new ThreadStart(() => sparse.computation(3)));
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


        /* Perform the main computation */
        //sparse.computation(0);


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


}