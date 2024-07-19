
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "raylib.h"
#include "raymath.h"

unsigned int SCREEN_WIDTH = 1000;
unsigned int SCREEN_HEIGHT = 800;

typedef struct {
    union {
        float c[4];
        struct { float x, y, z, w; };
    };
} Vec4;

void vec4_swap_cells(Vec4* v, int c1, int c2) {
    float ctemp = v->c[c1];
    v->c[c1] = v->c[c2];
    v->c[c2] = ctemp;
}

Vec4 vec4_scale(Vec4 vec, float scaler) {
    return (Vec4) {
        .x = scaler * vec.x,
        .y = scaler * vec.y,
        .z = scaler * vec.z,
        .w = scaler * vec.w,
    };
}

Vec4 vec4_add(Vec4 vec1, Vec4 vec2) {
    return (Vec4) {
        .x = vec1.x + vec2.x,
        .y = vec1.y + vec2.y,
        .z = vec1.z + vec2.z,
        .w = vec1.w + vec2.w,
    };
}

typedef struct {
    union {
        Vec4 row[4];
        struct { Vec4 r0, r1, r2, r3; };
    };
} Mat4;

void mat4_swap_rows(Mat4* m, int r1, int r2) {
    Vec4 rtemp = m->row[r1];
    m->row[r1] = m->row[r2];
    m->row[r2] = rtemp;
}

void print_mat4(Mat4 m) {
    printf(" ----------------------------------------------\n");
    printf("| %-10.2f %-10.2f %-10.2f %-10.2f\n", m.row[0].c[0], m.row[0].c[1], m.row[0].c[2], m.row[0].c[3]);
    printf("| %-10.2f %-10.2f %-10.2f %-10.2f\n", m.row[1].c[0], m.row[1].c[1], m.row[1].c[2], m.row[1].c[3]);
    printf("| %-10.2f %-10.2f %-10.2f %-10.2f\n", m.row[2].c[0], m.row[2].c[1], m.row[2].c[2], m.row[2].c[3]);
    printf("| %-10.2f %-10.2f %-10.2f %-10.2f\n", m.row[3].c[0], m.row[3].c[1], m.row[3].c[2], m.row[3].c[3]);
    printf(" ----------------------------------------------\n");
}

typedef struct {
    float a, b, c, d;
} CubicCurve;

typedef struct {
    Vector2 coord;
    Vector2 tangent;
} ControlPoint;

float f_cube(float x) {
    return x * x * x;
}

float f_sq(float x) {
    return x * x;
}

float cubic_curve_calculate(CubicCurve curve, float x) {
    return curve.a * f_cube(x)
        + curve.b * f_sq(x)
        + curve.c * x
        + curve.d;
}

float Vector2Slope(Vector2 vec) {
    return vec.y / vec.x;
}

void solve_cubic_curve(ControlPoint p1, ControlPoint p2, CubicCurve* curve) {
    const int DIM = 4;

    Mat4 A = {
        .r0 = {f_cube(p1.coord.x), f_sq(p1.coord.x), p1.coord.x, 1},
        .r1 = {f_cube(p2.coord.x), f_sq(p2.coord.x), p2.coord.x, 1},
        .r2 = {3 * f_sq(p1.coord.x), 2 * p1.coord.x, 1, 0},
        .r3 = {3 * f_sq(p2.coord.x), 2 * p2.coord.x, 1, 0},
    };
    Vec4 b = {p1.coord.y, p2.coord.y, Vector2Slope(p1.tangent), Vector2Slope(p2.tangent)};

    // if (A.row[0].c[0] == 0) {
    //     Vec4 rtemp = A.r0;
    //     A.r0 = A.r1;
    //     A.r1 = rtemp;
        
    //     float btemp = b.c[0];
    //     b.c[0] = b.c[1];
    //     b.c[1] = btemp;
    // }

    // if (A.row[0].c[0] == 0) {
    //     Vec4 rtemp = A.r0;
    //     A.r0 = A.r1;
    //     A.r1 = rtemp;
        
    //     float btemp = b.c[0];
    //     b.c[0] = b.c[1];
    //     b.c[1] = btemp;
    // }

    // A.row[1] = Vector4Add(A.row[1], Vector4Scale(A.row[0], -A.row[1].c[0] / A.row[0].c[0])); // j = 0, i = 1    --> col[0], row[1], using row[0]
    // A.row[2] = Vector4Add(A.row[2], Vector4Scale(A.row[0], -A.row[2].c[0] / A.row[0].c[0])); // j = 0, i = 2    --> col[0], row[2], using row[0]
    // A.row[3] = Vector4Add(A.row[3], Vector4Scale(A.row[0], -A.row[3].c[0] / A.row[0].c[0])); // j = 0, i = 3    --> col[0], row[3], using row[0]

    // A.row[2] = Vector4Add(A.row[2], Vector4Scale(A.row[1], -A.row[2].c[1] / A.row[1].c[1])); // j = 1, i = 2    --> col[1], row[2], using row[1]
    // A.row[3] = Vector4Add(A.row[3], Vector4Scale(A.row[1], -A.row[3].c[1] / A.row[1].c[1])); // j = 1, i = 3    --> col[1], row[3], using row[1]
    
    // A.row[3] = Vector4Add(A.row[3], Vector4Scale(A.row[2], -A.row[3].c[2] / A.row[2].c[2])); // j = 2, i = 3    --> col[2], row[3], using row[2]

    // printf("Constructed Matrix A:\n");
    // print_mat4(A);
    // printf("Constructed vector b:\n");
    // printf("[ %0.2f\t%0.2f\t%0.2f\t%0.2f ]\n", b.c[0], b.c[1], b.c[2], b.c[3]);

    for (int j = 0; j < DIM - 1; j++) {
        if (A.row[j].c[j] == 0) {
            for (int i = j+1; i < DIM; i++) {
                if (A.row[i].c[j] != 0) {
                    mat4_swap_rows(&A, j, i);
                    vec4_swap_cells(&b, j, i);
                    break;
                }
            }
        }
        for (int i = j+1; i < DIM; i++) {
            float factor = -A.row[i].c[j] / A.row[j].c[j];
            A.row[i] = vec4_add(A.row[i], vec4_scale(A.row[j], factor));
            b.c[i] += factor * b.c[j];
        }
    }

    // printf("Forward Eliminated A:\n");
    // print_mat4(A);
    // printf("Forward Eliminated b:\n");
    // printf("[ %0.2f\t%0.2f\t%0.2f\t%0.2f ]\n", b.c[0], b.c[1], b.c[2], b.c[3]);

    curve->d = b.c[3] / A.row[3].c[3];
    curve->c = (b.c[2] - A.row[2].c[3] * curve->d) / A.row[2].c[2];
    curve->b = (b.c[1] - A.row[1].c[3] * curve->d - A.row[1].c[2] * curve->c) / A.row[1].c[1];
    curve->a = (b.c[0] - A.row[0].c[3] * curve->d - A.row[0].c[2] * curve->c - A.row[0].c[1] * curve->b) / A.row[0].c[0];


    // printf("Solution:\n");
    // printf("[ %0.2f\t%0.2f\t%0.2f\t%0.2f ]\n", curve->a, curve->b, curve->c, curve->d);
}

const float DEFAULT_ARROW_HEAD_RADIUS = 20;
const float DEFAULT_ARROW_LENGTH = 100;

typedef struct {
    float head_radius;
    Vector2 base;
    float length;
    float angle;
} Arrow;

Vector2 arrow_get_head(Arrow arrow) {
    return Vector2Add(
        arrow.base,
        Vector2Rotate((Vector2) {arrow.length, 0}, arrow.angle)
    );
}

void arrow_draw(Arrow arrow, float thick, Color color) {
    Vector2 x_unit = {1, 0};
    Vector2 b = Vector2Zero();
    Vector2 h = {arrow.length, 0};
    Vector2 hu = Vector2Add(h, Vector2Scale(Vector2Rotate(x_unit, DEG2RAD * -150), arrow.head_radius));
    Vector2 hd = Vector2Add(h, Vector2Scale(Vector2Rotate(x_unit, DEG2RAD * -210), arrow.head_radius));

    b = Vector2Add(arrow.base, b);
    h = Vector2Add(arrow.base, Vector2Rotate(h, arrow.angle));
    hu = Vector2Add(arrow.base, Vector2Rotate(hu, arrow.angle));
    hd = Vector2Add(arrow.base, Vector2Rotate(hd, arrow.angle));

    DrawLineEx(b, h, thick, color);
    DrawLineEx(h, hu, thick, color);
    DrawLineEx(h, hd, thick, color);
}

typedef struct {
    int control_point_radius;
    Color control_point_idle_color;
    Color control_point_hold_color;
    Color curve_color;
} SplineStyle;

typedef struct {
    Vector2 begin_tangent;
    Vector2 end_tangent;
    int points_capacity;
    int n_points;
    ControlPoint* points;
    CubicCurve* curves; // n-1 curves but just have the +1 empty, it will make dev easier
} Spline;

Spline new_init_spline() {
    Spline s = {0};
    s.begin_tangent = (Vector2) {1, 0};
    s.end_tangent = (Vector2) {1, 0};
    return s;
}

void spline_push_back_point(Spline* spline, ControlPoint point) {
    const int INITIAL_CAPACITY = 10;
    const int GROWTH_FACTOR = 2;

    if (spline->points_capacity == 0) {
        spline->points_capacity = INITIAL_CAPACITY;
        spline->points = malloc(INITIAL_CAPACITY * sizeof(ControlPoint));
        spline->curves = malloc(INITIAL_CAPACITY * sizeof(CubicCurve));
    }
    else if (spline->n_points == spline->points_capacity) {
        int new_capacity = GROWTH_FACTOR * spline->points_capacity;
        printf("Realloc: cap: %d, new_cap: %d\n", spline->points_capacity, new_capacity);
        spline->points_capacity = new_capacity;
        spline->points = realloc(spline->points, new_capacity * sizeof(ControlPoint));
        spline->curves = realloc(spline->curves, new_capacity * sizeof(CubicCurve));
    }

    spline->points[spline->n_points] = point;
    // spline->curves[spline->n_points] = (CubicCurve){0}; // recalculate all curves after point push
    spline->n_points++;
}

void spline_calculate_curves(Spline* spline) {
    if (spline->n_points < 2) {
        // cannot have curve yet
        return;
    }

    // tangents for first and last point are controlable constraints
    // just like the point coordinates
    spline->points[0].tangent = spline->begin_tangent;
    spline->points[spline->n_points-1].tangent = spline->end_tangent;

    // calculate tangents for points in the middle
    for (int i = 1; i < spline->n_points-1; i++) {
        spline->points[i].tangent = Vector2Subtract(
            spline->points[i+1].coord,
            spline->points[i-1].coord
        );
    }

    for (int i = 0; i < spline->n_points-1; i++) {
        solve_cubic_curve(spline->points[i], spline->points[i+1], &spline->curves[i]);
    }
}

void spline_draw_curves(Spline* spline, SplineStyle style, int point_hold) {
    // printf("draw spline: %d\n", spline->n_points);

    // draw curves
    // printf("draw curves\n");
    const int curve_split = 100;
    float xc0, yc0, xc1, yc1;
    for (int i = 0; i < spline->n_points - 1; i++) {
        float x1 = spline->points[i].coord.x;
        float x2 = spline->points[i+1].coord.x;
        float step = (x2-x1)/curve_split;

        xc0 = x1;
        yc0 = cubic_curve_calculate(spline->curves[i], xc0);
        for (int j = 1; j <= curve_split; j++) {
            float xc1 = x1 + ((j+1) * step);
            float yc1 = cubic_curve_calculate(spline->curves[i], xc1);
            DrawLineEx(
                (Vector2) {xc0, yc0},
                (Vector2) {xc1, yc1},
                2,
                style.curve_color
            );
            xc0 = xc1;
            yc0 = yc1;
        }
    }

    // draw control points
    // printf("draw points\n");
    for (int i = 0; i < spline->n_points; i++) {
        int radius = style.control_point_radius;
        Color color = style.control_point_idle_color;
        if (i == point_hold) {
            radius *= 1.5;
            color = style.control_point_hold_color;
        }
        DrawCircleV(spline->points[i].coord, radius, color);
    }

    // draw tangents
    if (spline->n_points > 0) {
        float bt_angle = Vector2Angle((Vector2) {1, 0}, spline->begin_tangent);
        Arrow bt_neg_arrow = {
            .head_radius = DEFAULT_ARROW_HEAD_RADIUS,
            .base = spline->points[0].coord,
            .length = 100,
            .angle = bt_angle + PI
        };
        arrow_draw(bt_neg_arrow, 3, BLACK);

        if (spline->n_points > 1) {
            float bt_angle = Vector2Angle((Vector2) {1, 0}, spline->end_tangent);
            Arrow bt_neg_arrow = {
                .head_radius = DEFAULT_ARROW_HEAD_RADIUS,
                .base = spline->points[spline->n_points - 1].coord,
                .length = DEFAULT_ARROW_LENGTH,
                .angle = bt_angle + PI
            };
            arrow_draw(bt_neg_arrow, 3, BLACK);
        }
    }
}

int main() {

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Curve Maker");

    const int SCREEN_MARGIN_X = SCREEN_WIDTH / 8;
    const int SCREEN_MARGIN_Y = SCREEN_HEIGHT / 8;

    Camera2D camera = {0};
    camera.zoom = 1.0;
    camera.offset = (Vector2) {
        .x = SCREEN_MARGIN_X,
        .y = SCREEN_HEIGHT - SCREEN_MARGIN_Y,
    };

    const float x_axis_len = SCREEN_WIDTH - 2*SCREEN_MARGIN_X;
    const float y_axis_len = SCREEN_HEIGHT - 2*SCREEN_MARGIN_Y;

    Spline spline = new_init_spline();
    SplineStyle spline_style = {
        .control_point_radius = 10,
        .control_point_idle_color = RED,
        .control_point_hold_color = MAGENTA,
        .curve_color = BLUE,
    };

    int point_hold = -1;
    bool begin_tangent_hold = false;
    bool end_tangent_hold = false;

    while (!WindowShouldClose()) {
        
        Vector2 mouse = GetScreenToWorld2D(GetMousePosition(), camera);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (spline.n_points > 0) {
                Vector2 begin_neg_tangent_head = Vector2Add(
                    spline.points[0].coord,
                    Vector2Rotate(Vector2Scale(spline.begin_tangent, DEFAULT_ARROW_LENGTH), PI)
                );
                if (Vector2DistanceSqr(begin_neg_tangent_head, mouse) <= f_sq(DEFAULT_ARROW_HEAD_RADIUS)) {
                    begin_tangent_hold = true;
                    goto END_HOLD_CHECK;
                }

                if (spline.n_points > 1) {
                    Vector2 end_neg_tangent_head = Vector2Add(
                        spline.points[spline.n_points - 1].coord,
                        Vector2Rotate(Vector2Scale(spline.end_tangent, DEFAULT_ARROW_LENGTH), PI)
                    );
                    if (Vector2DistanceSqr(end_neg_tangent_head, mouse) <= f_sq(DEFAULT_ARROW_HEAD_RADIUS)) {
                        end_tangent_hold = true;
                        goto END_HOLD_CHECK;
                    }
                }
            }

            for (int i = 0; i < spline.n_points; i++) {
                if (Vector2DistanceSqr(spline.points[i].coord, mouse) <= f_sq(spline_style.control_point_radius)) {
                    point_hold = i;
                    goto END_HOLD_CHECK;
                }
            }

            float x_low_limit = (spline.n_points == 0) ? 0 : spline.points[spline.n_points - 1].coord.x;
            float x_high_limit = x_axis_len;
            Vector2 low_limit = {
                .x = x_low_limit + spline_style.control_point_radius,
                .y = -y_axis_len,
            };
            Vector2 high_limit = {
                .x = x_high_limit - spline_style.control_point_radius,
                .y = -spline_style.control_point_radius,
            };
            if (low_limit.x <= mouse.x && mouse.x <= high_limit.x
                && low_limit.y <= mouse.y && mouse.y <= high_limit.y
            ) {
                ControlPoint point = {0};
                point.coord = mouse;
                spline_push_back_point(&spline, point);
            }

            END_HOLD_CHECK:
        }
        else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            point_hold = -1;
            begin_tangent_hold = false;
            end_tangent_hold = false;
        }

        if (begin_tangent_hold) {
            spline.begin_tangent = Vector2Rotate(
                Vector2Normalize(Vector2Subtract(mouse, spline.points[0].coord)),
                PI
            );
        }
        else if (end_tangent_hold) {
            spline.end_tangent = Vector2Rotate(
                Vector2Normalize(Vector2Subtract(mouse, spline.points[spline.n_points - 1].coord)),
                PI
            );
        }
        else if (point_hold != -1) { // spline.n_points > 0
            spline.points[point_hold].coord = mouse;

            float x_low_limit, x_high_limit;
            if (point_hold == 0) {
                x_low_limit = 0;
                x_high_limit = (spline.n_points == 1) ? x_axis_len : spline.points[1].coord.x;
            }
            else if (point_hold == spline.n_points - 1) { // point_hold != 0 && spline.n_points > 1 => spline.n_points > 2
                x_low_limit = spline.points[point_hold - 1].coord.x;
                x_high_limit = x_axis_len;
            }
            else {
                x_low_limit = spline.points[point_hold - 1].coord.x;
                x_high_limit = spline.points[point_hold + 1].coord.x;
            }

            Vector2 low_limit = {
                .x = x_low_limit + spline_style.control_point_radius,
                .y = -y_axis_len,
            };
            Vector2 high_limit = {
                .x = x_high_limit - spline_style.control_point_radius,
                .y = -spline_style.control_point_radius,
            };
            spline.points[point_hold].coord.x = Clamp(spline.points[point_hold].coord.x, low_limit.x, high_limit.x);
            spline.points[point_hold].coord.y = Clamp(spline.points[point_hold].coord.y, low_limit.y, high_limit.y);
        }

        spline_calculate_curves(&spline);
        
        BeginDrawing();
            ClearBackground(BEIGE);
            BeginMode2D(camera);

                Vector2 zero = Vector2Zero();
                Vector2 x_unit = {1, 0};
                Vector2 y_unit = {0, -1};
                Vector2 x_axis_end = Vector2Scale(x_unit, x_axis_len);
                Vector2 y_axis_end = Vector2Scale(y_unit, y_axis_len);
                DrawLineEx(Vector2Zero(), x_axis_end, 5, WHITE);
                DrawLineEx(Vector2Zero(), y_axis_end, 5, WHITE);

                spline_draw_curves(&spline, spline_style, point_hold);

                // Arrow arrow1 = {
                //     .base = (Vector2) {100, -150},
                //     .length = 200,
                //     .angle = DEG2RAD * 30,
                // };
                // Arrow arrow2 = {
                //     .base = (Vector2) {100, -150},
                //     .length = 250,
                //     .angle = DEG2RAD * 180,
                // };
                
                // Arrow arrow3 = {
                //     .base = (Vector2) {100, -150},
                //     .length = 300,
                //     .angle = DEG2RAD * 0,
                // };

                // arrow_draw(arrow1);
                // arrow_draw(arrow2);
                // arrow_draw(arrow3);

            EndMode2D();

            const char* n_points_str = TextFormat("Points: %d", spline.n_points);
            DrawText(n_points_str, 15, 15, 20, RED);

        EndDrawing();
    }
    
    CloseWindow();

    return 0;
}