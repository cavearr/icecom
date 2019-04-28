/* test7.c

   Read/write data via serial I/O
   with measure baudrate

   This program is distributed under the GPL, version 2
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <ftdi.h>
#include <sys/time.h> // for clock_gettime()
#include <math.h>

static int exitRequested = 0;
/*
 * sigintHandler --
 *
 *    SIGINT handler, so we can gracefully exit when the user hits ctrl-C.
 */
static void
sigintHandler(int signum)
{
    exitRequested = 1;
}

int main(int argc, char **argv)
{
    struct ftdi_context *ftdi;
    unsigned char buf[1];
    //unsigned char buf[1024];
    int f = 0, i;
    int vid = 0x0403;
    int pid = 0x6010;
    int baudrate = 115200;
    int interface = INTERFACE_ANY;
    int do_write = 0;
    unsigned int pattern = 0xffff;
    int retval = EXIT_FAILURE;
    long  baudtimer=0;
    struct timeval start, end;
    long secs_used,micros_used;
    
    while ((i = getopt(argc, argv, "i:v:p:b:w::")) != -1)
    {
        switch (i)
        {
            case 'i': // 0=ANY, 1=A, 2=B, 3=C, 4=D
                interface = strtoul(optarg, NULL, 0);
                break;
            case 'v':
                vid = strtoul(optarg, NULL, 0);
                break;
            case 'p':
                pid = strtoul(optarg, NULL, 0);
                break;
            case 'b':
                baudrate = strtoul(optarg, NULL, 0);
                break;
            case 'w':
                do_write = 1;
                if (optarg)
                    pattern = strtoul(optarg, NULL, 0);
                if (pattern > 0xff)
                {
                    fprintf(stderr, "Please provide a 8 bit pattern\n");
                    exit(-1);
                }
                break;
            default:
                fprintf(stderr, "usage: %s [-i interface] [-v vid] [-p pid] [-b baudrate] [-w [pattern]]\n", *argv);
                exit(-1);
        }
    }

    // Init
    if ((ftdi = ftdi_new()) == 0)
    {
        fprintf(stderr, "ftdi_new failed\n");
        return EXIT_FAILURE;
    }

    if (!vid && !pid && (interface == INTERFACE_ANY))
    {
        ftdi_set_interface(ftdi, INTERFACE_ANY);
        struct ftdi_device_list *devlist;
        int res;
        if ((res = ftdi_usb_find_all(ftdi, &devlist, 0, 0)) < 0)
        {
            fprintf(stderr, "No FTDI with default VID/PID found\n");
            goto do_deinit;
        }
        if (res == 1)
        {
            f = ftdi_usb_open_dev(ftdi,  devlist[0].dev);
            if (f<0)
            {
                fprintf(stderr, "Unable to open device %d: (%s)",
                        i, ftdi_get_error_string(ftdi));
            }
        }
        ftdi_list_free(&devlist);
        if (res > 1)
        {
            fprintf(stderr, "%d Devices found, please select Device with VID/PID\n", res);
            /* TODO: List Devices*/
            goto do_deinit;
        }
        if (res == 0)
        {
            fprintf(stderr, "No Devices found with default VID/PID\n");
            goto do_deinit;
        }
    }
    else
    {
        // Select interface
        ftdi_set_interface(ftdi, interface);
        
        // Open device
        f = ftdi_usb_open(ftdi, vid, pid);
    }
    if (f < 0)
    {
        fprintf(stderr, "unable to open ftdi device: %d (%s)\n", f, ftdi_get_error_string(ftdi));
        exit(-1);
    }

    // Set baudrate
    f = ftdi_set_baudrate(ftdi, baudrate);
    if (f < 0)
    {
        fprintf(stderr, "unable to set baudrate: %d (%s)\n", f, ftdi_get_error_string(ftdi));
        exit(-1);
    }
    
    /* Set line parameters
     *
     * TODO: Make these parameters settable from the command line
     *
     * Parameters are choosen that sending a continous stream of 0x55 
     * should give a square wave
     *
     */
    f = ftdi_set_line_property(ftdi, 8, STOP_BIT_1, NONE);
    if (f < 0)
    {
        fprintf(stderr, "unable to set line parameters: %d (%s)\n", f, ftdi_get_error_string(ftdi));
        exit(-1);
    }
    
    if (do_write)
        for(i=0; i<1024; i++)
            buf[i] = pattern;

    signal(SIGINT, sigintHandler);

    gettimeofday(&start, NULL);
    while (!exitRequested)
    {
         
          
         if(baudtimer>1000000){
            gettimeofday(&end, NULL);
            
            secs_used=(end.tv_sec - start.tv_sec); //avoid overflow by subtracting first
    
            
            //   secs_used  ------- baudtimer bytes
            //       1s       ------- x bytes
            //
            //       x= 800/cpu_time_used
            if(secs_used>0.0)
            {
                fprintf(stderr, "\ntTime spent %lds ( %lfKB/s) => %ldbps\n", secs_used,(double)(baudtimer/secs_used  )/1024,(long)round((baudtimer/secs_used)*8)) ;
                fflush(stderr);
            }
            baudtimer=0;
            gettimeofday(&start, NULL);
        }

           
        if (do_write)
            f = ftdi_write_data(ftdi, buf, 
                                (baudrate/512 >sizeof(buf))?sizeof(buf):
                                (baudrate/512)?baudrate/512:1);
        else{
            f = ftdi_read_data(ftdi, buf, sizeof(buf));
        }
        if (f<0){
            //usleep(8);
        }else if(f> 0 && !do_write)
        {
            printf("%x ",buf[0]);
            baudtimer++;
        }
    }
    signal(SIGINT, SIG_DFL);
    retval =  EXIT_SUCCESS;
            
    ftdi_usb_close(ftdi);
    do_deinit:
    ftdi_free(ftdi);

    return retval;
}
