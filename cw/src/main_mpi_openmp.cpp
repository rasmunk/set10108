#define _USE_MATH_DEFINES

#include <math.h>   // smallpt, a Path Tracer by Kevin Beason, 2008
#include <stdlib.h> // Make : g++ -O3 -fopenmp smallpt.cpp -o smallpt
#include <stdio.h>  //        Remove "-fopenmp" for g++ version < 4.2
#include <chrono>
#include <omp.h>
#include <iostream>
#include <fstream>
#include <mpi.h>
#include <vector>
#include <thread>

using namespace std;
using namespace std::chrono;

double erand48(unsigned short xsubi[3]) {
    return (double) rand() / (double) RAND_MAX;
}

struct Vec
{        // Usage: time ./smallpt 5000 && xv image.ppm
    double x, y, z;                  // position, also color (r,g,b)
    Vec(double x_ = 0, double y_ = 0, double z_ = 0) {
        x = x_;
        y = y_;
        z = z_;
    }

    Vec operator+(const Vec &b) const { return Vec(x + b.x, y + b.y, z + b.z); }

    Vec operator-(const Vec &b) const { return Vec(x - b.x, y - b.y, z - b.z); }

    Vec operator*(double b) const { return Vec(x * b, y * b, z * b); }

    Vec mult(const Vec &b) const { return Vec(x * b.x, y * b.y, z * b.z); }

    Vec &norm() { return *this = *this * (1 / sqrt(x * x + y * y + z * z)); }

    double dot(const Vec &b) const { return x * b.x + y * b.y + z * b.z; } // cross:
    Vec operator%(Vec &b) { return Vec(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x); }
};

struct Ray
{
    Vec o, d;

    Ray(Vec o_, Vec d_) : o(o_), d(d_) {}
};

enum Refl_t
{
    DIFF, SPEC, REFR
};  // material types, used in radiance()

struct Sphere
{
    double rad;       // radius
    Vec p, e, c;      // position, emission, color
    Refl_t refl;      // reflection type (DIFFuse, SPECular, REFRactive)
    Sphere(double rad_, Vec p_, Vec e_, Vec c_, Refl_t refl_) :
            rad(rad_), p(p_), e(e_), c(c_), refl(refl_) {}

    double intersect(const Ray &r) const { // returns distance, 0 if nohit
        Vec op = p - r.o; // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
        double t, eps = 1e-4, b = op.dot(r.d), det = b * b - op.dot(op) + rad * rad;
        if (det < 0) return 0; else det = sqrt(det);
        return (t = b - det) > eps ? t : ((t = b + det) > eps ? t : 0);
    }
};

Sphere spheres[] = { //Scene: radius, position, emission, color, material
        Sphere(1e5, Vec(1e5 + 1, 40.8, 81.6), Vec(), Vec(.75, .25, .25), DIFF),//Left
        Sphere(1e5, Vec(-1e5 + 99, 40.8, 81.6), Vec(), Vec(.25, .25, .75), DIFF),//Rght
        Sphere(1e5, Vec(50, 40.8, 1e5), Vec(), Vec(.75, .75, .75), DIFF),//Back
        Sphere(1e5, Vec(50, 40.8, -1e5 + 170), Vec(), Vec(), DIFF),//Frnt
        Sphere(1e5, Vec(50, 1e5, 81.6), Vec(), Vec(.75, .75, .75), DIFF),//Botm
        Sphere(1e5, Vec(50, -1e5 + 81.6, 81.6), Vec(), Vec(.75, .75, .75), DIFF),//Top
        Sphere(16.5, Vec(27, 16.5, 47), Vec(), Vec(1, 1, 1) * .999, SPEC),//Mirr
        Sphere(16.5, Vec(73, 16.5, 78), Vec(), Vec(1, 1, 1) * .999, REFR),//Glas
        Sphere(600, Vec(50, 681.6 - .27, 81.6), Vec(12, 12, 12), Vec(), DIFF) //Lite
};

// Could be pushed to GPU
inline double clamp(double x) { return x < 0 ? 0 : x > 1 ? 1 : x; }

inline int toInt(double x) { return int(pow(clamp(x), 1 / 2.2) * 255 + .5); }

inline bool intersect(const Ray &r, double &t, int &id) {
    double n = sizeof(spheres) / sizeof(Sphere), d, inf = t = 1e20;
    for (int i = int(n); i--;)
        if ((d = spheres[i].intersect(r)) && d < t) {
            t = d;
            id = i;
        }
    return t < inf;
}

Vec radiance(const Ray &r_, int depth_, unsigned short *Xi) {
    double t;                               // distance to intersection
    int id = 0;                               // id of intersected object
    Ray r = r_;
    int depth = depth_;

    Vec cl(0, 0, 0);   // accumulated color
    Vec cf(1, 1, 1);  // accumulated reflectance
    while (true) {
        if (!intersect(r, t, id)) return cl; // if miss, return black
        const Sphere &obj = spheres[id];        // the hit object
        Vec x = r.o + r.d * t, n = (x - obj.p).norm(), nl = n.dot(r.d) < 0 ? n : n * -1, f = obj.c;
        double p = f.x > f.y && f.x > f.z ? f.x : f.y > f.z ? f.y : f.z; // max refl
        cl = cl + cf.mult(obj.e);
        if (++depth > 5) if (erand48(Xi) < p) f = f * (1 / p); else return cl; //R.R.
        cf = cf.mult(f);
        if (obj.refl == DIFF) {                  // Ideal DIFFUSE reflection
            double r1 = 2 * M_PI * erand48(Xi), r2 = erand48(Xi), r2s = sqrt(r2);
            Vec w = nl, u = ((fabs(w.x) > .1 ? Vec(0, 1) : Vec(1)) % w).norm(), v = w % u;
            Vec d = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).norm();
            //return obj.e + f.mult(radiance(Ray(x,d),depth,Xi));
            r = Ray(x, d);
            continue;
        } else if (obj.refl == SPEC) {           // Ideal SPECULAR reflection
            //return obj.e + f.mult(radiance(Ray(x,r.d-n*2*n.dot(r.d)),depth,Xi));
            r = Ray(x, r.d - n * 2 * n.dot(r.d));
            continue;
        }
        Ray reflRay(x, r.d - n * 2 * n.dot(r.d));     // Ideal dielectric REFRACTION
        bool into = n.dot(nl) > 0;                // Ray from outside going in?
        double nc = 1, nt = 1.5, nnt = into ? nc / nt : nt / nc, ddn = r.d.dot(nl), cos2t;
        if ((cos2t = 1 - nnt * nnt * (1 - ddn * ddn)) < 0) {    // Total internal reflection
            //return obj.e + f.mult(radiance(reflRay,depth,Xi));
            r = reflRay;
            continue;
        }
        Vec tdir = (r.d * nnt - n * ((into ? 1 : -1) * (ddn * nnt + sqrt(cos2t)))).norm();
        double a = nt - nc, b = nt + nc, R0 = a * a / (b * b), c = 1 - (into ? -ddn : tdir.dot(n));
        double Re = R0 + (1 - R0) * c * c * c * c * c, Tr = 1 - Re, P = .25 + .5 * Re, RP = Re / P, TP = Tr / (1 - P);
        // return obj.e + f.mult(erand48(Xi)<P ?
        //                       radiance(reflRay,    depth,Xi)*RP:
        //                       radiance(Ray(x,tdir),depth,Xi)*TP);
        if (erand48(Xi) < P) {
            cf = cf * RP;
            r = reflRay;
        } else {
            cf = cf * TP;
            r = Ray(x, tdir);
        }
        continue;
    }
}


MPI_Datatype create_mpi_ray_type() {
    const int items = 3;
    int block_length[] = {1, 1, 1};
    MPI_Datatype types[items] = {MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE};
    MPI_Datatype mpi_ray_type;
    MPI_Aint offsets[items];

    offsets[0] = (MPI_Aint) offsetof(struct Vec, x);
    offsets[1] = (MPI_Aint) offsetof(struct Vec, y);
    offsets[2] = (MPI_Aint) offsetof(struct Vec, z);

    MPI_Type_create_struct(items, block_length, offsets, types, &mpi_ray_type);
    MPI_Type_commit(&mpi_ray_type);
    return mpi_ray_type;
}

void save_image(const vector<Vec> &image_vector, int width, int height) {
    // Write image to PPM file.
    FILE *f = fopen("image_mpi.ppm", "w");
    fprintf(f, "P3\n%d %d\n%d\n", width, height, 255);
    for (const auto &vec : image_vector) {
        fprintf(f, "%d %d %d ", toInt(vec.x), toInt(vec.y), toInt(vec.z));
    }
    fclose(f);
}

void run(int sample_size, int width, int height, int num_procs, int my_rank, int threads) {
    // Initialize
    int w = width, h = height, samps = sample_size; // # samples
    Ray cam(Vec(50, 52, 295.6), Vec(0, -0.042612, -1).norm()); // cam pos, dir
    Vec cx = Vec(w * .5135 / h), cy = (cx % cam.d).norm() * .5135, r;

    // Setup local mpi process variables
    int node_height = (h / num_procs);
    int node_height_start = (node_height * my_rank);
    int node_height_top = (node_height_start + node_height);
    int node_size = w * node_height;
    int total_size = h * w;

    Vec *myC = new Vec[node_size];

    // Run
    omp_set_num_threads(threads);
#pragma omp parallel for schedule(dynamic) private(r)
    for (int y = node_height_start;
         y < node_height_top; y++) {                       // Loop over image cols // Run 768 times
        fprintf(stderr, "\rRendering (%d spp) %5.2f%%", samps * 4, 100. * y / (h - 1));
        for (unsigned short x = 0, Xi[3] = {0, 0, y * y * y}; x < w; x++)   // Loop rows // run 768 * 1024
        {
            for (int sy = 0, i = (node_height_top - y - 1) * w + x;
                 sy < 2; sy++)     // 2x2 subpixel rows // run 768 * 1024 * 2
            {
                for (int sx = 0; sx < 2; sx++, r = Vec()) {        // 2x2 subpixel cols -> run 768 * 1024 * 2 * 2
                    for (int s = 0;
                         s < samps; s++) { // Run for a max of 16 times each time -> run 768 * 1024 * 2 * 2 * 16
                        double r1 = 2 * erand48(Xi), dx = r1 < 1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1);
                        double r2 = 2 * erand48(Xi), dy = r2 < 1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2);
                        Vec d = cx * (((sx + .5 + dx) / 2 + x) / w - .5) +
                                cy * (((sy + .5 + dy) / 2 + y) / h - .5) + cam.d;
                        r = r + radiance(Ray(cam.o + d * 140, d.norm()), 0, Xi) * (1. / samps);
                    } // Camera rays are pushed ^^^^^ forward to start in interior
                    myC[i] = myC[i] + Vec(clamp(r.x), clamp(r.y), clamp(r.z)) * .25;
                }
            }
        }
    }
    // End
    Vec *total_vector;

    // Master node allocates space to receive results
    if (my_rank == 0) {
        total_vector = new Vec[total_size];
    }
    auto mpi_ray_type = create_mpi_ray_type();

    // Gather results in
    MPI_Gather(&myC[0], node_size, mpi_ray_type, &total_vector[0], node_size, mpi_ray_type, 0, MPI_COMM_WORLD);
    if (my_rank == 0) {
        // Let master output image
        int partial_size = (total_size / num_procs);
        // vectors are received in reverse order
        vector<Vec> image;
        for (int i = num_procs; i > 0; --i) {
            int partial_bottom = ((i - 1) * partial_size);
            for (int y = 0; y < partial_size; ++y) {
                image.push_back(total_vector[partial_bottom + y]);
            }
        }
        delete[] total_vector;
        save_image(image, width, height);
    }

    // Sync all processes before moving on to the next render -> make sure no one starts on the next loop before all are ready
    // MPI_Barrier(MPI_COMM_WORLD);
    delete[] myC;
    MPI_Type_free(&mpi_ray_type);
}

int main(int argc, char *argv[]) {

    // not inside runRay because -> can't initialize and finalize mpi more than once
    auto result = MPI_Init(nullptr, nullptr);
    if (result != MPI_SUCCESS) {
        cout << "ERROR - initialising MPI" << endl;
        MPI_Abort(MPI_COMM_WORLD, result);
    }

    int num_procs, my_rank;

    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    int sample_size = 8;
    int width = 1920;
    int height = 1080;
    int iterations = 1;
    int threads = 8;

    for (int i = 0; i < iterations; ++i) {
        // Make sure everyone is ready
        MPI_Barrier(MPI_COMM_WORLD);
        run(sample_size, width, height, num_procs, my_rank, threads);
    }

    MPI_Finalize();
    return 0;
}