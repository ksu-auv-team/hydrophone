/**
 *  Test program that calculates FFTs for an 88.1kHz input file
 *  Outputs the 4 loudest frequency bands from 25kHz-40kHz and their fft values
 *
 *  NOTE: Input file must be 88.1kHz for this to work properly
 *
 *  Writen for KSU AUV team June 2016
 *
 *  Authors:
 *  Philip Nickerson
 *
 *
 *
 *
 */

#include <fftw3.h>
#include <sndfile.h>
#include <math.h>

int main (int argc, char *argv [])
{
    //Variables relating to the sound files
    char        *infilename;        //input file name
    SNDFILE     *infile = NULL;     //input file data
    FILE        *outfile = NULL;    //if we were writing a file, the output file's
                                    //data
    SF_INFO     sfinfo;             //file metadate

    infile = sf_open("fft_test.flac", SFM_READ, &sfinfo);   //88.1kHz flac file
                                                            //for analysis

    int N = pow(2, 12);                                     //Number of samples
                                                            //per seek frame for
                                                            //FFT calculations

    //interprets file data and prints it to the console
    //Outputs the 4 loudest frequency bands from 25kHz-40kHz and their fft values
    for(int i = 0; i < sfinfo.frames; i += 88)
    {
        double samples[N];                      //stores data for current frame

        sf_seek(infile, i, SEEK_SET);           //seeks to frame i
        sf_read_double(infile, samples, N);     //reads the input file at the current
                                                //frame with N samples and stores
                                                //data to samples array

        //calculate the FFTs and store them to out
        fftw_complex out[N];
        fftw_plan p;
        p = fftw_plan_dft_r2c_1d(N, samples, out, FFTW_ESTIMATE);

        fftw_execute(p);

        //prevent memory leaks
        fftw_destroy_plan(p);

        /*
        //print every bit of relevent data
        for (int i=0; i<N/2; i++)
        {
            printf("%i\t%f\n", (int)(i*21.533203125), sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]));
        }
        */

        //variables for sorting the data
        double total = 0;
        int prevmod = 1000;
        int band = 0;
        double highest[4][2] = {{0, 0},
                                {0, 0},
                                {0, 0},
                                {0, 0}};

        //sort the data, grouping FFTs in 1kHz bands, offset by 500Hz
        for(int i=0; i<N/2; i++)
        {
            total += sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);

            if (((int)(i*21.533203125) + 500) % 1000 < prevmod && i != 0)
            {
                //printf("%i\t%f\n", band * 1000, total);

                //Quick and dirty hadcoded sorting
                if(band != 0)
                {
                    if(total > highest[0][1])
                    {
                        highest[3][0] = highest[2][0];
                        highest[3][1] = highest[2][1];

                        highest[2][0] = highest[1][0];
                        highest[2][1] = highest[1][1];

                        highest[1][0] = highest[0][0];
                        highest[1][1] = highest[0][1];

                        highest[0][0] = band * 1000;
                        highest[0][1] = total;
                    }
                    else if(total > highest[1][1])
                    {
                        highest[3][0] = highest[2][0];
                        highest[3][1] = highest[2][1];

                        highest[2][0] = highest[1][0];
                        highest[2][1] = highest[1][1];

                        highest[1][0] = band * 1000;
                        highest[1][1] = total;
                    }
                    else if(total > highest[2][1])
                    {
                        highest[3][0] = highest[2][0];
                        highest[3][1] = highest[2][1];

                        highest[2][0] = band * 1000;
                        highest[2][1] = total;
                    }
                    else if(total > highest[3][1])
                    {
                        highest[3][0] = band * 1000;
                        highest[3][1] = total;
                    }
                }
                band++;
                total = 0;
            }
            prevmod = ((int)(i*21.533203125) + 500) % 1000;
        }

        //print data in a somewhat neat format
        printf("time: %i\n", i);
        printf("%f\t%f\n", highest[3][0], highest[3][1]);
        printf("%f\t%f\n", highest[2][0], highest[2][1]);
        printf("%f\t%f\n", highest[1][0], highest[1][1]);
        printf("%f\t%f\n", highest[0][0], highest[0][1]);
    }

    //prevent memory leaks
    sf_close (infile);

    //it worked!
    return 0;
}
