using System;
using System.Diagnostics;
using System.Threading;
using System.IO;
using System.Reflection;

/* Class monte CombinationaloLogic */
public class CombinationaloLogic
{

    /* Buffer that coantains the string that we want to count the number of 1 */
    private char []buffer;
    private int num_of_ones;
    private int buffer_size;
    private int workers;

    /* Constructor */
    public CombinationaloLogic(int workers)
    {
        this.buffer_size = Buffer_Size();
        buffer = new char[buffer_size];
        NUM_OF_ONES = 0;
        this.workers = workers;
    }

    /* Function that computes the number of '1' */
    public int CountNumber_of_Ones(int worker_id)
    {
        /* obtain reference to currently executing thread */
        Thread current = Thread.CurrentThread;

        /* Variable declaration */
        int counter = 0;
        int rounds = 10000;
        bool stop = false;

        int start = (int) Math.Floor( (double) (buffer_size * worker_id / workers));
        int end = (int) Math.Floor( (double) (buffer_size * (worker_id + 1) / workers));

        //Console.WriteLine("Start : " + start);
        //Console.WriteLine("End : " + end);
        //Console.WriteLine("BufferSize : " + buffer_size);

        /* Do the computation for this number of rounds */
        while (--rounds > 0)
        {
            /* Main computation */
            for (int i = start; i < end; i++)
                if (buffer[i] == '1' && stop == false)
                    counter++;

            /* We do not compute the number of ones on each iteration */
            stop = true;
        }

        //Console.WriteLine("NUmber of ones is : " + counter);
        Console.WriteLine(current.Name + " computed  " + counter + " number of ones");
        NUM_OF_ONES += counter;

        return counter;
    }

    /* Property NUM_OF_CHARS */
    public int NUM_OF_ONES
    {
        get
        {
            return num_of_ones;
        }

        set
        {
            num_of_ones = value;
        }
    }

    /* print the buffer */
    public void print()
    {
        Console.WriteLine("Printing buffer :");
        for (int i = 0; i < buffer.Length; i++)
            Console.WriteLine(buffer[i]);
    }

    /* The size of the buffer */
    public int Buffer_Size()
    {
        /* Get the size of the file */
        FileInfo f = new FileInfo("c:/users/toshiba/documents/visual studio 2010/Projects/CombinationalLogic/CombinationalLogic/file.txt");
        int fileLength = (int)f.Length;
        //Console.WriteLine("Buffer Size is : " + fileLength);

        /* Return the size of file */
        return fileLength;
    }

    /* Read the file that has word that we want to 
     * count the occurences of 1 */
    public void readFile()
    {
        /* Declare the counter of the buffer */
        int counter = 0;

        /* Create a StreamReader Object */
        StreamReader reader = new StreamReader("c:/users/toshiba/documents/visual studio 2010/Projects/CombinationalLogic/CombinationalLogic/file.txt");
        /* Attempt to read the file, if it exists */
        try
        {
            /* read character by character ntile the end of file */
            do
                {
                    char s = (char)reader.Read();
                    buffer[counter++] = s;
                    //Console.WriteLine(s);
                } while (!reader.EndOfStream);

        }
        catch (Exception e)
        {
            /* Let the user know what went wrong. */
            Console.WriteLine("The file could not be read:");
            Console.WriteLine(e.Message);
        }

        //Console.WriteLine("eeeeeeeee : " + counter);
    }

    /* Main function */
    public static void Main()
    {

        /* Create object */
        CombinationaloLogic cLogic = new CombinationaloLogic(4);

        /* Read the specified file */
        cLogic.readFile();
        //cL.print();
        //int start = 0;
        //int end = 15;
        //cL.CountNumber_of_Ones(0);

        /* Create the workers */
        Thread worker1 = new Thread(new ThreadStart( () => cLogic.CountNumber_of_Ones(0) ));
        worker1.Name = "worker1";

        /* Create the workers */
        Thread worker2 = new Thread(new ThreadStart(() => cLogic.CountNumber_of_Ones(1)));
        worker2.Name = "worker2";

        /* Create the workers */
        Thread worker3 = new Thread(new ThreadStart(() => cLogic.CountNumber_of_Ones(2)));
        worker3.Name = "worker3";

        /* Create the workers */
        Thread worker4 = new Thread(new ThreadStart(() => cLogic.CountNumber_of_Ones(3)));
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

        Console.WriteLine("Final number of ones after computation is " + cLogic.NUM_OF_ONES);
    }

}//End class CombinationaloLogic

