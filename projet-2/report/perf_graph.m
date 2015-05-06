figure();
%xlabel(Nombre de threads de calculs.);
threads = [1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 27, 28];
time_one = 2243.383;
time = [    
            time_one,
            1236.066,
            674.754,
            485.515,
            456.589,
            440.532,
            427.871,
            422.068,
            415.408,
            410.614,
            408.480,
            404.092,
            402.664,
            399.713,
            399.648,
            396.879
       ];
speedup = time_one./time;
plot(threads, speedup);