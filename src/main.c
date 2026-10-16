#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <SDL2/SDL.h>

#include "vector3.h"
#include "sphere.h"
#include "light.h"
#include "triangle.h"
#include "edge.h"
#include "delaunay2.h"
#include "colour.h"
#include "half.h"
#include "delaunay3.h"

#define REFRESH_DELAY 50000 //50000
#define DISPLAY_DELAY 0 //5 second display time
#define SCREEN_J 1501 //Number of columns
#define SCREEN_I 1501 //Number of lines
//Note: having odd numbers allows for a perfect center
#define BRIGHTNESS_RANGE 8
#define FRAMES 1
#define PIXEL_SIZE 1
#define BIG_ARRAY 3999999
const char pixels[BRIGHTNESS_RANGE] = {' ', '-', '~', '+', 'o', '8', '#', '@'};
const double pi = 3.141593;

//Prepare bounding boxes
const double x_min = 1500; //1700
const double y_min = -3000; //-1300
const double z_min = -3300; //-4700
const double x_max = 20000; //4900
const double y_max = 3000; //1300
const double z_max = 1500; // 1900 (Sacre coeur(alpha2500)) 
const int box_x_amount = 10; //12
const int box_y_amount = 15; //22
const int box_z_amount = 2; //8

struct World {
    int n_spheres;
    int n_lights;
    int n_triangles;
    sphere* spheres;
    light* lights;
    triangle* triangles;
};
typedef struct World world;

colour** create_screen(void);
void delete_screen(double** screen);
vector3** create_camera(void);
void delete_camera(vector3** camera);
void initialise_camera(vector3** camera, double fov, double y_rot);
char brightness_to_pixel(double brightness);
void render(colour** screen, vector3** camera, world* world, int pixel_size);
void render_accelerated(colour** screen, vector3** camera, world* world, int pixel_size);
int extract_points(FILE* fh, vector3* points);

bool box_intersects_vector(vector3 box_center, double x_r, double y_r, double z_r, vector3 v);
bool box_intersects_triangle(vector3 box_center, double x_r, double y_r, double z_r, triangle t);
void tetgen_write_points(vector3* points, int n_points);
int tetgen_read_triangles(triangle* triangles, vector3* points);
void tetgen_alpha_filter(double alpha, vector3* points);
int tetgen_alpha_filter_triangles(triangle* triangles, double alpha, vector3* points);
int tetgen_alpha_filter_triangles_ext(triangle* triangles, double alpha_min, double alpha_max, vector3* points);
double tetrahedron_volume(vector3 a, vector3 b, vector3 c, vector3 d);
double tetrahedron_circumsphere_volume(vector3 a, vector3 b, vector3 c, vector3 d);
double tetrahedron_edge_length(vector3 a, vector3 b, vector3 c, vector3 d);

int main(void) {

    //Bounding triangle access
    edge* e0 = d2_bounding_triangle();

/*
    //Read file points
    FILE* fh = fopen("data/mont_blanc_50k.txt", "r");
    vector3* points = (vector3*)malloc(sizeof(vector3) * BIG_ARRAY);
    int n_points = extract_points(fh, points);
    printf("n_points : %i\n", n_points);
*/

/*2D DT
    for (int i = 0; i < n_points; i++) {
        vector3 p = points[i];
        d2_insert(p, e0);
    }
    fclose(fh);
*/

    //Random
    srand(time(NULL));
    /*
    for (int i = 0; i < 600; i++) {
        int x = 1000 + rand() % 20000;
        int y = -3000 + rand() % 6000;
        int z = -1000 + rand() % (y / 4);
        vector3 p = v_create(x, y, z); 
        d2_insert(p, e0);
    }
    */
    for (int x0 = 1000; x0 <= 10000; x0 = x0 + 500) {
        for (int y0 = -3000; y0 <= 6000; y0 = y0 + 250) {
            int x = x0 + rand() % 200;
            int y  = y0 + rand() % 200;
            int z = -1000 + rand() % (abs(y) / 4 + 20);
            vector3 p = v_create(x, y, z); 
            d2_insert(p, e0);
        }
    }

/*
    //Output to node file
    tetgen_write_points(points, n_points);
*/
    //Define screen
    colour** screen = create_screen();

    //Define camera
    vector3** camera = create_camera();
    initialise_camera(camera, pi / 4, -pi / 14);

    //Define light objects
    light light_1 = l_create(v_create(9000, 0, 5000), 1000);

    //Define the world
    const int n_spheres = 0;
    const int n_lights = 1;
    sphere* spheres = (sphere*)malloc(sizeof(sphere) * n_spheres);
    light* lights = (light*)malloc(sizeof(light) * n_lights);

    //Fill the world
    triangle* triangles = (triangle*)malloc(sizeof(triangle) * BIG_ARRAY);
    /*
    //Tetgen business
    int res = system("./tetgen/tetgen -n ./tetgen/points.node");
    assert(res == 0);
    int n_triangles = tetgen_read_triangles(triangles, points);
    printf("n_triangles: %i\n", n_triangles);
    */

    const int n_triangles = d2_extract_triangles(triangles, e_rot(e0));
    //const int n_triangles = 1;
    //triangles[0] = triangle_1;
    printf("n_triangles: %i\n", n_triangles);


    printf("Faire le rendu ? ");
    fflush(stdout);
    char answer_render;
    scanf(" %c", &answer_render);
    if (answer_render != 'y') return 0;

    //spheres[0] = sphere_1; DO NOT UNCOMMENT IF THERE IS NO SPHERE
    lights[0] = light_1;

    world world_1 = {
        .n_spheres = n_spheres,
        .n_lights = n_lights,
        .n_triangles = n_triangles,
        .spheres = spheres,
        .lights = lights,
        .triangles = triangles
    };

    //SDL business
    SDL_Window *window = SDL_CreateWindow(
        "Pixel Display", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        SCREEN_J, SCREEN_I, 0
    );
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //Black background
    SDL_RenderClear(renderer);

    //Frame loop
    for (int frame = 0; frame < FRAMES; frame++) {
        //Render world into screen
        clock_t begin = clock();
        render_accelerated(screen, camera, &world_1, PIXEL_SIZE + FRAMES - frame - 1);
        clock_t end = clock();
        //Display screen
        for (int i = 0; i < SCREEN_I; i++) { //Iterate over row (vertically)
            for (int j = 0; j < SCREEN_J; j++) { //Iterate over column (horizontally)
                colour c = screen[i][j];
                SDL_SetRenderDrawColor(renderer,
                    (int) c.r * c.brightness,
                    (int) c.g * c.brightness,
                    (int) c.b * c.brightness,
                    255
                );
                SDL_RenderDrawPoint(renderer, j, i);
            }
        }
        usleep(REFRESH_DELAY);
        //SDL business
        SDL_RenderPresent(renderer);
        //Time spent
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("Rendu terminé en %lfs\n", time_spent);
        //Ask for next frame
        printf("Faire le rendu suivant ? ");
        fflush(stdout);
        char answer;
        scanf(" %c", &answer);
        if (answer != 'y') break;
    }

    //SDL business
    SDL_Delay(DISPLAY_DELAY);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    //Delete objects
    delete_camera(camera);
    free(spheres);
    free(lights);
    free(triangles);

    return 0;
}

colour** create_screen(void) {
    colour** screen = (colour**)malloc(sizeof(colour*) * SCREEN_J); //2D array that holds lines
    for (int i = 0; i < SCREEN_I; i++) {
        colour* line = (colour*)malloc(sizeof(colour) * SCREEN_I); //1D array representing a line
        screen[i] = line;
    }
    return screen;
}

void delete_screen(double** screen) {
    for (int i = 0; i < SCREEN_I; i++) {
        free(screen[i]); //Delete line
    }
    free(screen);
}

vector3** create_camera(void) {
    vector3** camera = (vector3**)malloc(sizeof(vector3*)*SCREEN_J); //2D array that holds lines
    for (int i = 0; i < SCREEN_I; i++) {
        vector3* line = (vector3*)malloc(sizeof(vector3)*SCREEN_I); //1D array representing a line
        camera[i] = line;
    }
    return camera;
}

void delete_camera(vector3** camera) {
    for (int i = 0; i < SCREEN_I; i++) {
        free(camera[i]); //Delete line
    }
    free(camera);
}

void initialise_camera(vector3** camera, double fov, double y_rot) {
    //fov is the angle range in radians
    for (int i = 0; i < SCREEN_I; i++) {
        for (int j = 0; j < SCREEN_J; j++) {
            //Controls the up pitch - positive goes down - 0 is at the very top - pi is at the very bottom
            double theta = (pi / 2) - (fov / 2) + fov * (i / (double)SCREEN_I);
            //Controls the yaw - positive goes left - 0 is facing forwards - pi is facing backwards
            double phi = (fov / 2) - fov * (j / (double)SCREEN_J);
            //Initialise vectors correctly
            vector3 v = {.x = 0.0, .y = 0.0, .z = 0.0};
            v.x = sin(theta) * cos(phi);
            v.y = sin(theta) * sin(phi);
            v.z = cos(theta);
            v = v_normalise(v);
            v = v_rotate(v, 0, y_rot, 0);
            camera[i][j] = v;
            //printf("i=%i j=%i theta=%lf phi=%lf x=%lf y=%lf z=%lf\n", i, j, theta, phi, v.x, v.y, v.z);
        }
    }
}

char brightness_to_pixel(double brightness) {
    double brightness_scaled = brightness * (BRIGHTNESS_RANGE - 1);
    return pixels[(int)brightness_scaled];
}

void render(colour** screen, vector3** camera, world* world, int pixel_size) {
    int done = 0;
    for (int i = 0; i < SCREEN_I / pixel_size; i++) { //Iterate over row (vertically)

        //For the RENDERING text
        int done_new = (100 * i) / (SCREEN_I / pixel_size) + 2;
        if (done_new > done) {
            printf("\rRENDERING --- %i%%", done);
            fflush(stdout);
            done = done_new;
        }

        //Avoid unresponsive window TODO does this work???
        SDL_Delay(1);

        for (int j = 0; j < SCREEN_J / pixel_size; j++) { //Iterate over column (horizontally)
            vector3 v = camera[i * pixel_size + pixel_size / 2][j * pixel_size + pixel_size / 2];

            //Spheres
            double s_min_distance = 9999.0;
            bool s_intersection_exists = false;
            sphere s_min;
            for (int k = 0; k < world->n_spheres; k++) {
                sphere s = world->spheres[k];
                bool intersects = s_intersects(s, v);
                if (intersects) {
                    //Calculate distance between source and point of intersection
                    double intersects_distance = s_intersection_distance(s, v);
                    if (intersects_distance < s_min_distance) {
                        s_min_distance = intersects_distance;
                        s_intersection_exists = true;
                        s_min = s;
                    }
                }
            }

            //Triangles
            double t_min_distance = 9999.0;
            bool t_intersection_exists = false;
            triangle t_min;
            for (int k = 0; k < world->n_triangles; k++) {
                triangle t = world->triangles[k];
                bool intersects = t_intersects(t, v);
                if (intersects) {
                    //Calculate distance between source and point of intersection
                    double intersects_distance = t_intersection_distance(t, v);
                    if (intersects_distance < t_min_distance) {
                        t_min_distance = intersects_distance;
                        t_intersection_exists = true;
                        t_min = t;
                    }
                }
            }

            //Determine pixel to print
            double brightness;
            colour c;
            if (!s_intersection_exists && !t_intersection_exists) { //No intersection
                brightness = 0;
            }
            else if (s_min_distance < t_min_distance) { //Sphere intersection
                //Calculate distance between light source and point of intersection
                double intersects_distance = s_intersection_distance(s_min, v);
                double light_distance = v_norm(v_sub(world->lights[0].position, v_mul(v, intersects_distance)));
                //Calculate normal vector
                vector3 normal = s_intersection_normal(s_min, v);
                //Calculate other useful vectors
                vector3 intersection = v_mul(v, intersects_distance);
                vector3 light_to_intersection = v_sub(intersection, world->lights[0].position);
                //Apply shaders to get brightness
                if (v_dot(normal, light_to_intersection) > 0) {
                    //Intersection point is not illuminated
                    brightness = 0;
                }
                else {
                    brightness = v_dot(normal, v_normalise(light_to_intersection));
                    brightness = fabs(brightness);
                    brightness = fmin(1, brightness);
                }
            }
            else { //Triangle intersection
                //Calculate distance between light source and point of intersection
                double intersects_distance = t_intersection_distance(t_min, v);
                double light_distance = v_norm(v_sub(world->lights[0].position, v_mul(v, intersects_distance)));
                //Calculate normal vector
                vector3 normal = t_intersection_normal(t_min, v);
                //Calculate other useful vectors
                vector3 intersection = v_mul(v, intersects_distance);
                vector3 light_to_intersection = v_sub(intersection, world->lights[0].position);
                //Apply shaders to get brightness
                if (v_dot(normal, light_to_intersection) > 0) {
                    //Intersection point is not illuminated
                    brightness = 0;
                }
                else {
                    brightness = v_dot(normal, v_normalise(light_to_intersection));
                    brightness = fabs(brightness);
                    brightness = sqrt(brightness);
                    brightness = fmin(1, brightness);
                }
                c = t_min.c;
            }

            //Set pixel
            colour pixel = c;
            pixel.brightness = brightness;
            for (int k = 0; k < pixel_size; k++) {
                for (int l = 0; l < pixel_size; l++) {
                    int ii = pixel_size * i + k;
                    int jj = pixel_size * j + l;
                    if (ii < SCREEN_I && jj < SCREEN_J) {
                        screen[ii][jj] = pixel;
                    }
                }
            }
        }
    }
    printf("\n"); //For the RENDERING text
}

void render_accelerated(colour** screen, vector3** camera, world* world, int pixel_size) {

    //Prepare bounding boxes - moved to top
    int box_amount = box_x_amount * box_y_amount * box_z_amount;

    double box_x_radius = (int)(x_max - x_min) / (box_x_amount * 2);
    double box_y_radius = (int)(y_max - y_min) / (box_y_amount * 2);
    double box_z_radius = (int)(z_max - z_min) / (box_z_amount * 2);
    //printf("Box radiuses: %lf, %lf, %lf\n", box_x_radius, box_y_radius, box_z_radius);
    double box_x_origin = x_min;
    double box_y_origin = y_min;
    double box_z_origin = z_min;
    vector3* boxes = (vector3*)malloc(sizeof(vector3) * box_amount);
    int c = 0;
    for (int i = 0; i < box_x_amount; i++) {
        for (int j = 0; j < box_y_amount; j++) {
            for (int k = 0; k < box_z_amount; k++) {
                vector3 box_center = v_create(
                    box_x_origin + box_x_radius + 2 * box_x_radius * i,
                    box_y_origin + box_y_radius + 2 * box_y_radius * j,
                    box_z_origin + box_z_radius + 2 * box_z_radius * k
                );
                boxes[c] = box_center;
                c = c + 1;
            }
        }
    }

    //Fill bounding boxes
    triangle** boxes_triangles = (triangle**)malloc(sizeof(triangle*) * box_amount);
    int* boxes_size = (int*)malloc(sizeof(int) * box_amount);
    for (int l = 0; l < box_amount; l++) {
        vector3 box_center = boxes[l];
        triangle* box_triangles = (triangle*)malloc(sizeof(triangle) * BIG_ARRAY); //TODO this in unreliable
        int box_size = 0;
        for (int k = 0; k < world->n_triangles; k++) {
            triangle t = world->triangles[k];
            if (box_intersects_triangle(box_center, box_x_radius, box_y_radius, box_z_radius, t)) {
                box_triangles[box_size] = t;
                box_size = box_size + 1;
            }
        }
        boxes_size[l] = box_size;
        boxes_triangles[l] = box_triangles;
        //printf("Box n°%i has %i triangles\n", l, box_size);
    }

    int done = 0;
    int box_intersections = 0; //STATS
    int triangle_tests = 0; //STATS
    for (int i = 0; i < SCREEN_I / pixel_size; i++) { //Iterate over row (vertically)

        //For the RENDERING text
        int done_new = (100 * i) / (SCREEN_I / pixel_size) + 2;
        if (done_new > done) {
            printf("\rRENDU EN COURS --- %i%%", done);
            fflush(stdout);
            done = done_new;
        }

        //Avoid unresponsive window TODO does this work???
        SDL_Delay(1);

        for (int j = 0; j < SCREEN_J / pixel_size; j++) { //Iterate over column (horizontally)
            vector3 v = camera[i * pixel_size + pixel_size / 2][j * pixel_size + pixel_size / 2];

            //Spheres
            double s_min_distance = 9999.0;
            bool s_intersection_exists = false;
            sphere s_min;
            for (int k = 0; k < world->n_spheres; k++) {
                sphere s = world->spheres[k];
                bool intersects = s_intersects(s, v);
                if (intersects) {
                    //Calculate distance between source and point of intersection
                    double intersects_distance = s_intersection_distance(s, v);
                    if (intersects_distance < s_min_distance) {
                        s_min_distance = intersects_distance;
                        s_intersection_exists = true;
                        s_min = s;
                    }
                }
            }

            //Triangles
            double t_min_distance = 9999.0;
            bool t_intersection_exists = false;
            triangle t_min;

            //Accelerated rendering
            for (int l = 0; l < box_amount; l++) {
                //Does the ray intersect this box? Using ray-AABB intersection
                vector3 box_center = boxes[l];
                
                if (box_intersects_vector(box_center, box_x_radius, box_y_radius, box_z_radius, v)) {
                    //It does, check all triangles inside
                    //printf("Pixel (%i, %i) intersects box %i\n", i, j, l);
                    int box_size = boxes_size[l];
                    box_intersections = box_intersections + 1; //STATS
                    triangle_tests = triangle_tests + box_size; //STATS
                    for (int k = 0; k < box_size; k++) {
                        triangle t = boxes_triangles[l][k];
                        bool intersects = t_intersects(t, v);
                        if (intersects) {
                            //Calculate distance between source and point of intersection
                            double intersects_distance = t_intersection_distance(t, v);
                            if (intersects_distance < t_min_distance) {
                                t_min_distance = intersects_distance;
                                t_intersection_exists = true;
                                t_min = t;
                            }
                        }
                    }
                }
            }

            //Determine pixel to print
            double brightness;
            colour c = c_create_white();
            if (!s_intersection_exists && !t_intersection_exists) { //No intersection
                brightness = 1; //0
            }
            else if (s_min_distance < t_min_distance) { //Sphere intersection
                //Calculate distance between light source and point of intersection
                double intersects_distance = s_intersection_distance(s_min, v);
                double light_distance = v_norm(v_sub(world->lights[0].position, v_mul(v, intersects_distance)));
                //Calculate normal vector
                vector3 normal = s_intersection_normal(s_min, v);
                //Calculate other useful vectors
                vector3 intersection = v_mul(v, intersects_distance);
                vector3 light_to_intersection = v_sub(intersection, world->lights[0].position);
                //Apply shaders to get brightness
                if (v_dot(normal, light_to_intersection) > 0) {
                    //Intersection point is not illuminated
                    brightness = 0;
                }
                else {
                    brightness = v_dot(normal, v_normalise(light_to_intersection));
                    brightness = fabs(brightness);
                    brightness = fmin(1, brightness);
                }
            }
            else { //Triangle intersection
                //Calculate distance between light source and point of intersection
                double intersects_distance = t_intersection_distance(t_min, v);
                double light_distance = v_norm(v_sub(world->lights[0].position, v_mul(v, intersects_distance)));
                //Calculate normal vector
                vector3 normal = t_intersection_normal(t_min, v);
                //Calculate other useful vectors
                vector3 intersection = v_mul(v, intersects_distance);
                vector3 light_to_intersection = v_sub(intersection, world->lights[0].position);
                //Apply shaders to get brightness
                if (v_dot(normal, light_to_intersection) > 0) {
                    //Intersection point is not illuminated
                    brightness = 0.3; //0
                }
                else {
                    brightness = v_dot(normal, v_normalise(light_to_intersection));
                    brightness = fabs(brightness);
                    brightness = sqrt(brightness);

                    //BG
                    //brightness = brightness + 0.00003 * (intersects_distance - 1000.0);

                    brightness = fmax(0.3, brightness);
                    brightness = fmin(1, brightness);
                }
                c = t_min.c;
            }

            //Set pixel
            colour pixel = c;
            pixel.brightness = brightness;
            for (int k = 0; k < pixel_size; k++) {
                for (int l = 0; l < pixel_size; l++) {
                    int ii = pixel_size * i + k;
                    int jj = pixel_size * j + l;
                    if (ii < SCREEN_I && jj < SCREEN_J) {
                        screen[ii][jj] = pixel;
                    }
                }
            }
        }
    }
    printf("\n"); //For the RENDERING text
    printf("Calculs d'intersection rayon/boîte : %i\n", box_intersections);
    printf("Calculs d'intersection rayon/triangle : %i\n", triangle_tests);
}

int extract_points(FILE* fh, vector3* points) {
    int n_points = 0;
    double x;
    double y;
    double z;
    while (fscanf(fh, "%lf %lf %lf%*[^\n]", &x, &y, &z) == 3) {
        //Fill the table
        vector3 v = v_create(x, y, z);
        points[n_points] = v;
        n_points = n_points + 1;
        //printf("Read %i: %lf, %lf, %lf\n", n_points, x, y, z);
    }

    vector3 v_mini = v_min(points, n_points);
    vector3 v_maxi = v_max(points, n_points);
    printf("min and max and delta and moy:\n");
    v_print(v_mini);
    v_print(v_maxi);
    vector3 v_delta = v_sub(v_maxi, v_mini);
    vector3 v_moy = v_mul(v_add(v_mini, v_maxi), 0.5);
    v_print(v_delta);
    v_print(v_moy);

    //vector3 v_offset = v_create(3300, 0, -3300);
    vector3 v_offset = v_create(3300, 0, -2500);
    double expand = 0.4;
    for (int i = 0; i < n_points; i++) {
        vector3 v = points[i];
        vector3 u = v_add(v_mul(v_sub(v, v_moy), expand), v_offset);
        points[i] = u;
        //printf("%lf %lf %lf\n", u.x, u.y, u.z);
    }

    vector3 v_mini_2 = v_min(points, n_points);
    vector3 v_maxi_2 = v_max(points, n_points);
    printf("min and max:\n");
    v_print(v_mini_2);
    v_print(v_maxi_2);

    return n_points;
}

bool box_intersects_vector(vector3 box_center, double x_r, double y_r, double z_r, vector3 v) {
    double qx_1 = (box_center.x - x_r) / v.x;
    double qx_2 = (box_center.x + x_r) / v.x;
    double qx_min = fmin(qx_1, qx_2);
    double qx_max = fmax(qx_1, qx_2);
    double qy_1 = (box_center.y - y_r) / v.y;
    double qy_2 = (box_center.y + y_r) / v.y;
    double qy_min = fmin(qy_1, qy_2);
    double qy_max = fmax(qy_1, qy_2);
    double qz_1 = (box_center.z - z_r) / v.z;
    double qz_2 = (box_center.z + z_r) / v.z;
    double qz_min = fmin(qz_1, qz_2);
    double qz_max = fmax(qz_1, qz_2);
    double q_min = fmax(fmax(qx_min, qy_min), qz_min);
    double q_max = fmin(fmin(qx_max, qy_max), qz_max);
    return q_min <= q_max;
}

bool box_intersects_triangle(vector3 box_center, double x_r, double y_r, double z_r, triangle t) {
    return 
        (abs(t.point_1.x - box_center.x) < x_r &&
        abs(t.point_1.y - box_center.y) < y_r &&
        abs(t.point_1.z - box_center.z) < z_r)
        ||
        (abs(t.point_2.x - box_center.x) < x_r &&
        abs(t.point_2.y - box_center.y) < y_r &&
        abs(t.point_2.z - box_center.z) < z_r)
        ||
        (abs(t.point_3.x - box_center.x) < x_r &&
        abs(t.point_3.y - box_center.y) < y_r &&
        abs(t.point_3.z - box_center.z) < z_r);
}

void tetgen_write_points(vector3* points, int n_points) {
    FILE* fh = fopen("tetgen/points.node", "w");
    fprintf(fh, "%d 3 0 0\n", n_points);
    for (int i = 0; i < n_points; i++) {
        fprintf(fh, "%d %.17g %.17g %.17g\n", i, points[i].x, points[i].y, points[i].z);
    }
    fclose(fh);
}


int tetgen_read_triangles(triangle* triangles, vector3* points) {
    FILE* fh = fopen("tetgen/points.1.face", "r");
    int n_triangles;
    int has_boundary;
    fscanf(fh, "%d %d", &n_triangles, &has_boundary);
    for (int i = 0; i < n_triangles; i++) {
        int n_triangle;
        int k1;
        int k2;
        int k3;
        if (has_boundary) {
            int marker;
            fscanf(fh, "%d %d %d %d %d", &n_triangle, &k1, &k2, &k3, &marker);
        } else {
            fscanf(fh, "%d %d %d %d", &n_triangle, &k1, &k2, &k3);
        }
        triangles[i] = t_create(points[k1], points[k2], points[k3]);
    }
    fclose(fh);
    return n_triangles;
}

struct Quadruplet {
    int k1;
    int k2;
    int k3;
    int k4;
};

void tetgen_alpha_filter(double alpha, vector3* points) {
    struct Quadruplet* tets_new = (struct Quadruplet*)malloc(BIG_ARRAY * sizeof(struct Quadruplet));
    int n_tets_new = 0;
    FILE* f1 = fopen("tetgen/points.1.ele", "r");
    int n_tets;
    int bin;
    fscanf(f1, "%d %d %d", &n_tets, &bin, &bin);
    for (int i = 0; i < n_tets; i++) {
        int n_tet;
        int k1;
        int k2;
        int k3;
        int k4;
        fscanf(f1, "%d %d %d %d %d", &n_tet, &k1, &k2, &k3, &k4);
        double volume = tetrahedron_volume(points[k1], points[k2], points[k3], points[k4]);
        if (fabs(volume) < alpha) {
            //printf("Tetra %d volume: %lf\n", n_tet, volume);
            tets_new[n_tets_new] = (struct Quadruplet){.k1 = k1, .k2 = k2, .k3 = k3, .k4 = k4};
            n_tets_new = n_tets_new + 1;
        }
    }
    fclose(f1);
    FILE* f2 = fopen("tetgen/tets.ele", "w");
    fprintf(f2, "%d 4 0\n", n_tets_new);
    for (int i = 0; i < n_tets_new; i++) {
        fprintf(f2, "%d %d %d %d %d\n", i, tets_new[i].k1, tets_new[i].k2, tets_new[i].k3, tets_new[i].k4);
    }
    fclose(f2);
}

int tetgen_alpha_filter_triangles(triangle* triangles, double alpha, vector3* points) {
    int n_triangles = 0;
    FILE* f1 = fopen("tetgen/points.1.ele", "r");
    int n_tets;
    int bin;
    fscanf(f1, "%d %d %d", &n_tets, &bin, &bin);
    for (int i = 0; i < n_tets; i++) {
        int n_tet;
        int k1;
        int k2;
        int k3;
        int k4;
        fscanf(f1, "%d %d %d %d %d", &n_tet, &k1, &k2, &k3, &k4);
        double volume = tetrahedron_volume(points[k1], points[k2], points[k3], points[k4]);
        if (fabs(volume) < alpha) {
            //printf("Tetra %d volume: %lf\n", n_tet, volume);
            vector3 a = points[k1];
            vector3 b = points[k2];
            vector3 c = points[k3];
            vector3 d = points[k4];
            triangle t1 = t_create(a, b, c);
            triangle t2 = t_create(a, b, d);
            triangle t3 = t_create(c, a, d);
            triangle t4 = t_create(b, c, d);
            triangles[n_triangles] = t1;
            triangles[n_triangles + 1] = t2;
            triangles[n_triangles + 2] = t3;
            triangles[n_triangles + 3] = t4;
            n_triangles = n_triangles + 4;
        }
    }
    fclose(f1);
    return n_triangles;
}

struct Ele {
    int k1;
    int k2;
    int k3;
    int k4;
    int n1;
    int n2;
    int n3;
    int n4;
    bool keep;
};

int tetgen_alpha_filter_triangles_ext(triangle* triangles, double alpha_min, double alpha_max, vector3* points) {

    //Read and initialise tets, set points and keep
    struct Ele* tets = (struct Ele*)malloc(sizeof(struct Ele) * BIG_ARRAY);
    FILE* f1 = fopen("tetgen/points.1.ele", "r");
    int n_tets;
    int bin;
    fscanf(f1, "%d %d %d", &n_tets, &bin, &bin);
    for (int i = 0; i < n_tets; i++) {
        int n_tet;
        int k1;
        int k2;
        int k3;
        int k4;
        fscanf(f1, "%d %d %d %d %d", &n_tet, &k1, &k2, &k3, &k4);
        struct Ele tet = (struct Ele){
            .k1 = k1, .k2 = k2, .k3 = k3, .k4 = k4,
            .n1 = 0, .n2 = 0, .n3 = 0, .n4 = 0,
            .keep = false
        };
        //Modify this function for measuring
        double volume = tetrahedron_circumsphere_volume(points[k1], points[k2], points[k3], points[k4]);
        if (fabs(volume) > alpha_min && fabs(volume) < alpha_max) {
            tet.keep = true;
        }
        tets[i] = tet;
    }
    fclose(f1);

    //Read all neighbours, set neighbours
    FILE* f2 = fopen("tetgen/points.1.neigh", "r");
    fscanf(f2, "%d %d", &n_tets, &bin);
    for (int i = 0; i < n_tets; i++) {
        int n_tet;
        int n1;
        int n2;
        int n3;
        int n4;
        fscanf(f1, "%d %d %d %d %d", &n_tet, &n1, &n2, &n3, &n4);
        tets[i].n1 = n1;
        tets[i].n2 = n2;
        tets[i].n3 = n3;
        tets[i].n4 = n4;
    }
    fclose(f2);

    //Extract triangles of ext
    int n_triangles = 0;
    for (int i = 0; i < n_tets; i++) {
        struct Ele tet = tets[i];
        if (tet.keep) {
            //printf("Tetra %d volume: %lf\n", n_tet, volume);
            vector3 a = points[tet.k1];
            vector3 b = points[tet.k2];
            vector3 c = points[tet.k3];
            vector3 d = points[tet.k4];
            triangle t1 = t_create(a, b, c);
            triangle t2 = t_create(a, b, d);
            triangle t3 = t_create(c, a, d);
            triangle t4 = t_create(b, c, d);
            if (!tets[tet.n4].keep) {
                triangles[n_triangles] = t1;
                n_triangles = n_triangles + 1;
            }
            if (!tets[tet.n3].keep) {
                triangles[n_triangles] = t2;
                n_triangles = n_triangles + 1;
            }
            if (!tets[tet.n2].keep) {
                triangles[n_triangles] = t3;
                n_triangles = n_triangles + 1;
            }
            if (!tets[tet.n1].keep) {
                triangles[n_triangles] = t4;
                n_triangles = n_triangles + 1;
            }
        }
    }
    return n_triangles;
}

double tetrahedron_volume(vector3 a, vector3 b, vector3 c, vector3 d) {
    vector3 ad = v_create(a.x - d.x, a.y - d.y, a.z - d.z);
    vector3 bd = v_create(b.x - d.x, b.y - d.y, b.z - d.z);
    vector3 cd = v_create(c.x - d.x, c.y - d.y, c.z - d.z);
    vector3 cross = v_cross(bd, cd);
    double dot = v_dot(ad, cross);
    return dot / 6.0;
}

double tetrahedron_circumsphere_volume(vector3 a, vector3 b, vector3 c, vector3 d) {
    vector3 u = {b.x - a.x, b.y - a.y, b.z - a.z};
    vector3 v = {c.x - a.x, c.y - a.y, c.z - a.z};
    vector3 w = {d.x - a.x, d.y - a.y, d.z - a.z};
    
    double uu = v_dot(u, u);
    double vv = v_dot(v, v);
    double ww = v_dot(w, w);
    
    vector3 cross_vw = v_cross(v, w);
    vector3 cross_wu = v_cross(w, u);
    vector3 cross_uv = v_cross(u, v);
    
    double denom = 2.0 * v_dot(u, cross_vw);
    if (fabs(denom) < 1e-10) {
        return 0.0;
    }
    
    vector3 numerator;
    numerator.x = uu * cross_vw.x + vv * cross_wu.x + ww * cross_uv.x;
    numerator.y = uu * cross_vw.y + vv * cross_wu.y + ww * cross_uv.y;
    numerator.z = uu * cross_vw.z + vv * cross_wu.z + ww * cross_uv.z;
    
    vector3 o;
    o.x = a.x + numerator.x / denom;
    o.y = a.y + numerator.y / denom;
    o.z = a.z + numerator.z / denom;
    
    double r = v_norm(v_sub(o, a));
    double volume = (4.0 / 3.0) * pi * r * r * r;
    return volume;
}

double tetrahedron_edge_length(vector3 a, vector3 b, vector3 c, vector3 d) {
    return
        v_norm(v_sub(a, b)) +
        v_norm(v_sub(b, c)) +
        v_norm(v_sub(c, a)) +
        v_norm(v_sub(a, d)) +
        v_norm(v_sub(b, d)) +
        v_norm(v_sub(c, d));        
}
