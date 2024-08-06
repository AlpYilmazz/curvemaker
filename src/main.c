
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "raylib.h"
#include "raymath.h"

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

typedef struct {
    float in_line_thick;
    float out_line_thick;
    Color color;
} Border;

typedef struct {
    Vector2 origin;         // in global coordinates
    Vector2 orientation;    // 1: normal, -1: reverse
    float scale;
} Axis2D;

float axis2d_scale_out(Axis2D axis, float f) {
    return f * axis.scale;
}

float axis2d_scale_into(Axis2D axis, float f) {
    return f / axis.scale;
}

Vector2 axis2d_orient_out(Axis2D axis, Vector2 v) {
    return Vector2Multiply(v, axis.orientation);
}

Vector2 axis2d_orient_into(Axis2D axis, Vector2 v) {
    return Vector2Multiply(v, axis.orientation);
}

Vector2 axis2d_shift_out(Axis2D axis, Vector2 v) {
    return Vector2Add(
        axis.origin,
        Vector2Scale(Vector2Multiply(v, axis.orientation), axis.scale)
    );
}

Vector2 axis2d_shift_into(Axis2D axis, Vector2 v) {
    return Vector2Multiply(
        axis.orientation,
        Vector2Scale(Vector2Subtract(v, axis.origin), 1.0/axis.scale)
    );
}

typedef struct {
    int id;
    bool disabled;
    Rectangle rect;
    Axis2D axis;
    Border border;
    Color background;
} UICanvas;

void ui_canvas_draw(UICanvas* canvas) {
    // Draw Background
    DrawRectangleRec(canvas->rect, canvas->background);

    // Draw Border
    if (canvas->border.in_line_thick > 0.0 || canvas->border.out_line_thick > 0.0) {
        Rectangle rect = canvas->rect;
        rect.x -= canvas->border.out_line_thick;
        rect.y -= canvas->border.out_line_thick;
        rect.width += 2 * canvas->border.out_line_thick;
        rect.height += 2 * canvas->border.out_line_thick;
        float line_thick = canvas->border.in_line_thick + canvas->border.out_line_thick;
        DrawRectangleLinesEx(rect, line_thick, canvas->border.color);
    }
}

typedef struct {
    UICanvas canvas;
    // Vector2 axis_margin;
    // Vector2 axis_len;
    Vector2 axis_len; // axis_divide;
    Color axis_color;
} Graph2DCanvas;

Graph2DCanvas graph2d_canvas_create_default(int id, Rectangle rect) {
    Vector2 axis_margin_global = {
        .x = rect.width/10.0,
        .y = rect.height/10.0,
    };
    Vector2 axis_len_global = {
        .x = rect.width - 2*axis_margin_global.x,
        .y = rect.height - 2*axis_margin_global.y,
    };
    float scale = 1;
    Vector2 axis_len_local = Vector2Scale(axis_len_global, 1.0/scale);

    return (Graph2DCanvas) {
        .canvas = (UICanvas) {
            .id = id,
            .disabled = false,
            .rect = rect,
            .axis = (Axis2D) {
                .origin = (Vector2) {
                    .x = rect.x + axis_margin_global.x,
                    .y = rect.y + rect.height - axis_margin_global.y,
                },
                .orientation = {1, -1},
                .scale = scale,
            },
            .border = (Border) {
                .in_line_thick = 0,
                .out_line_thick = 2,
                .color = BLUE,
            },
            .background = WHITE,
        },
        // .axis_margin = axis_margin,
        // .axis_len = axis_len,
        .axis_len = axis_len_local,
        .axis_color = BLACK,
    };
}

void graph2d_canvas_set_rect(Graph2DCanvas* graph2d_canvas, Rectangle rect) {
    float scale = graph2d_canvas->canvas.axis.scale;
    
    Vector2 axis_margin_global = {
        .x = rect.width/10.0,
        .y = rect.height/10.0,
    };
    Vector2 axis_len_global = {
        .x = rect.width - 2*axis_margin_global.x,
        .y = rect.height - 2*axis_margin_global.y,
    };
    Vector2 axis_len_local = Vector2Scale(axis_len_global, 1.0/scale);

    graph2d_canvas->canvas.rect = rect;
    graph2d_canvas->canvas.axis.origin = (Vector2) {
        .x = rect.x + axis_margin_global.x,
        .y = rect.y + rect.height - axis_margin_global.y,
    };
    graph2d_canvas->axis_len = axis_len_local;
}

void graph2d_canvas_draw(Graph2DCanvas* graph_canvas) {
    // Draw Canvas
    ui_canvas_draw(&graph_canvas->canvas);

    // Draw Axis
    // canvas.axis.origin is shifted onto graph origin
    Vector2 x_axis_end = { graph_canvas->axis_len.x, 0 };
    Vector2 y_axis_end = { 0, graph_canvas->axis_len.y };

    Vector2 zero = axis2d_shift_out(graph_canvas->canvas.axis, Vector2Zero());
    x_axis_end = axis2d_shift_out(graph_canvas->canvas.axis, x_axis_end);
    y_axis_end = axis2d_shift_out(graph_canvas->canvas.axis, y_axis_end);
    DrawLineEx(zero, x_axis_end, 5, graph_canvas->axis_color);
    DrawLineEx(zero, y_axis_end, 5, graph_canvas->axis_color);
}

struct Global {
    unsigned int SCREEN_WIDTH;
    unsigned int SCREEN_HEIGHT;
    Axis2D AXIS;
} GLOBAL = {
    .SCREEN_WIDTH = 1000,
    .SCREEN_HEIGHT = 800,
    .AXIS = {
        .origin = {0, 0},
        .orientation = {1, 1},
        .scale = 1.0,
    },
};

// const float DEFAULT_ARROW_HEAD_RADIUS = 20;
// const float DEFAULT_ARROW_LENGTH = 100;

typedef struct {
    float head_radius;
    Vector2 base;
    // float length;
    // float angle;
    Vector2 direction; // normalized
    float length;
} Arrow;

Vector2 arrow_get_head(Arrow arrow) {
    return Vector2Add(
        arrow.base,
        Vector2Scale(arrow.direction, arrow.length)
    );
}

void arrow_draw(Arrow arrow, float thick, Color color) {
    Vector2 b = Vector2Zero();
    Vector2 h = Vector2Scale(arrow.direction, arrow.length);
    Vector2 hu = Vector2Add(h, Vector2Scale(Vector2Rotate(arrow.direction, DEG2RAD * -150), arrow.head_radius));
    Vector2 hd = Vector2Add(h, Vector2Scale(Vector2Rotate(arrow.direction, DEG2RAD * -210), arrow.head_radius));

    b = Vector2Add(arrow.base, b);
    h = Vector2Add(arrow.base, h);
    hu = Vector2Add(arrow.base, hu);
    hd = Vector2Add(arrow.base, hd);

    // Vector2 x_unit = {1, 0};
    // Vector2 b = Vector2Zero();
    // Vector2 h = {arrow_length, 0};
    // Vector2 hu = Vector2Add(h, Vector2Scale(Vector2Rotate(x_unit, DEG2RAD * -150), arrow.head_radius));
    // Vector2 hd = Vector2Add(h, Vector2Scale(Vector2Rotate(x_unit, DEG2RAD * -210), arrow.head_radius));

    // b = Vector2Add(arrow.base, b);
    // h = Vector2Add(arrow.base, Vector2Rotate(h, arrow.angle));
    // hu = Vector2Add(arrow.base, Vector2Rotate(hu, arrow.angle));
    // hd = Vector2Add(arrow.base, Vector2Rotate(hd, arrow.angle));

    // b = axis2d_shift_out(axis, b);
    // h = axis2d_shift_out(axis, h);
    // hu = axis2d_shift_out(axis, hu);
    // hd = axis2d_shift_out(axis, hd);

    DrawLineEx(b, h, thick, color);
    DrawLineEx(h, hu, thick, color);
    DrawLineEx(h, hd, thick, color);
}

typedef struct {
    float arrow_length;
    float arrow_head_radius;
    float control_point_radius;
    Color control_point_idle_color;
    Color control_point_hold_color;
    Color curve_color;
} SplineStyle;

typedef struct {
    Vector2 begin_tangent_normalized;
    Vector2 end_tangent_normalized;
    int points_capacity;
    int n_points;
    ControlPoint* points;
    CubicCurve* curves; // n-1 curves but just have the +1 empty, it will make dev easier
} Spline;

Spline new_init_spline() {
    Spline s = {0};
    s.begin_tangent_normalized = (Vector2) {1, 0};
    s.end_tangent_normalized = (Vector2) {1, 0};
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
    spline->points[0].tangent = spline->begin_tangent_normalized;
    spline->points[spline->n_points-1].tangent = spline->end_tangent_normalized;

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

void spline_draw_curves(Spline* spline, SplineStyle style, Axis2D axis, int point_hold) {
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
                axis2d_shift_out(axis, (Vector2) {xc0, yc0}),
                axis2d_shift_out(axis, (Vector2) {xc1, yc1}),
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
        DrawCircleV(
            axis2d_shift_out(axis, spline->points[i].coord),
            radius,
            color
        );
    }

    // draw tangents
    if (spline->n_points > 0) {
        // float bt_angle = Vector2Angle((Vector2) {1, 0}, spline->begin_tangent_normalized);
        Arrow bt_neg_arrow = {
            .head_radius = style.arrow_head_radius,
            .base = axis2d_shift_out(axis, spline->points[0].coord),
            .direction = axis2d_orient_out(axis, spline->begin_tangent_normalized),
            .length = style.arrow_length,
        };
        arrow_draw(bt_neg_arrow, 3, BLACK);

        if (spline->n_points > 1) {
            // float bt_angle = Vector2Angle((Vector2) {1, 0}, spline->end_tangent_normalized);
            Arrow bt_neg_arrow = {
                .head_radius = style.arrow_head_radius,
                .base = axis2d_shift_out(axis, spline->points[spline->n_points - 1].coord),
                .direction = axis2d_orient_out(axis, spline->end_tangent_normalized),
                .length = style.arrow_length,
            };
            arrow_draw(bt_neg_arrow, 3, BLACK);
        }
    }
}

typedef struct {
    Graph2DCanvas graph2d_canvas;
    Spline spline;
    SplineStyle spline_style;
    // -- System Data --
    Vector2 relative_mouse;
    bool spline_updated;
    int point_hold;
    bool begin_tangent_hold;
    bool end_tangent_hold;
} SplineEntity;

SplineEntity spline_entity_create_default(int id) {

    Rectangle graph2d_canvas_rect = {
        .x = 100,
        .y = 100,
        .width = 800,
        .height = 400,
    };
    Graph2DCanvas graph2d_canvas = graph2d_canvas_create_default(id, graph2d_canvas_rect);

    Spline spline = new_init_spline();

    SplineStyle spline_style = {
        .arrow_head_radius = 20,
        .arrow_length = 100,
        .control_point_radius = 10,
        .control_point_idle_color = RED,
        .control_point_hold_color = MAGENTA,
        .curve_color = BLUE,
    };

    return (SplineEntity) {
        .graph2d_canvas = graph2d_canvas,
        .spline = spline,
        .spline_style = spline_style,
        // -- System Data --
        .relative_mouse = {0, 0},
        .spline_updated = false,
        .point_hold = -1,
        .begin_tangent_hold = false,
        .end_tangent_hold = false,
    };
}

void spline_entity_set_input(SplineEntity* spline_entity, Camera2D camera) {
    Vector2 mouse = GetScreenToWorld2D(GetMousePosition(), camera);
    spline_entity->relative_mouse = axis2d_shift_into(spline_entity->graph2d_canvas.canvas.axis, mouse);
}

void spline_entity_process_input(SplineEntity* spline_entity) {
    Graph2DCanvas* graph2d_canvas = &spline_entity->graph2d_canvas;
    Spline* spline = &spline_entity->spline;
    SplineStyle* spline_style = &spline_entity->spline_style;
    bool* set_spline_updated = &spline_entity->spline_updated;
    int* set_point_hold = &spline_entity->point_hold;
    bool* set_begin_tangent_hold = &spline_entity->begin_tangent_hold;
    bool* set_end_tangent_hold = &spline_entity->end_tangent_hold;

    Vector2 relative_mouse = spline_entity->relative_mouse;

    Axis2D axis = graph2d_canvas->canvas.axis;
    float x_axis_len = graph2d_canvas->axis_len.x;
    float y_axis_len = graph2d_canvas->axis_len.y;
    float local_spline_style_arrow_length = axis2d_scale_into(axis, spline_style->arrow_length);
    float local_spline_style_arrow_head_radius = axis2d_scale_into(axis, spline_style->arrow_head_radius);
    float local_spline_style_control_point_radius = axis2d_scale_into(axis, spline_style->control_point_radius);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        printf("mouse: %0.2f, %0.2f\n", relative_mouse.x, relative_mouse.y);

        if (spline->n_points > 0) {
            Vector2 begin_neg_tangent_head = Vector2Add(
                spline->points[0].coord,
                Vector2Scale(spline->begin_tangent_normalized, local_spline_style_arrow_length)
            );
            if (Vector2DistanceSqr(begin_neg_tangent_head, relative_mouse) <= f_sq(local_spline_style_arrow_head_radius)) {
                *set_begin_tangent_hold = true;
                goto END_HOLD_CHECK;
            }

            if (spline->n_points > 1) {
                Vector2 end_neg_tangent_head = Vector2Add(
                    spline->points[spline->n_points - 1].coord,
                    Vector2Scale(spline->end_tangent_normalized, local_spline_style_arrow_length)
                );
                if (Vector2DistanceSqr(end_neg_tangent_head, relative_mouse) <= f_sq(local_spline_style_arrow_head_radius)) {
                    *set_end_tangent_hold = true;
                    goto END_HOLD_CHECK;
                }
            }
        }

        for (int i = 0; i < spline->n_points; i++) {
            Vector2 coord = spline->points[i].coord;
            if (Vector2DistanceSqr(coord, relative_mouse) <= f_sq(local_spline_style_control_point_radius)) {
                *set_point_hold = i;
                goto END_HOLD_CHECK;
            }
        }

        float x_low_limit = (spline->n_points == 0) ? 0 : spline->points[spline->n_points - 1].coord.x;
        float x_high_limit = x_axis_len;
        float limit_margin = local_spline_style_control_point_radius;
        Vector2 low_limit = {
            .x = x_low_limit + limit_margin,
            .y = limit_margin,
        };
        Vector2 high_limit = {
            .x = x_high_limit - limit_margin,
            .y = y_axis_len - limit_margin,
        };

        printf("low_limit: %0.2f, %0.2f\n", low_limit.x, low_limit.y);
        printf("high_limit: %0.2f, %0.2f\n", high_limit.x, high_limit.y);

        if (low_limit.x <= relative_mouse.x && relative_mouse.x <= high_limit.x
            && low_limit.y <= relative_mouse.y && relative_mouse.y <= high_limit.y
        ) {
            *set_spline_updated = true;
            ControlPoint point = {0};
            point.coord = relative_mouse;
            spline_push_back_point(spline, point);
        }

        END_HOLD_CHECK:
    }
    else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        *set_point_hold = -1;
        *set_begin_tangent_hold = false;
        *set_end_tangent_hold = false;
    }
}

void spline_entity_update(SplineEntity* spline_entity) {
    Graph2DCanvas* graph2d_canvas = &spline_entity->graph2d_canvas;
    Spline* spline = &spline_entity->spline;
    SplineStyle* spline_style = &spline_entity->spline_style;
    bool* set_spline_updated = &spline_entity->spline_updated;

    Vector2 relative_mouse = spline_entity->relative_mouse;
    int point_hold = spline_entity->point_hold;
    bool begin_tangent_hold = spline_entity->begin_tangent_hold;
    bool end_tangent_hold = spline_entity->end_tangent_hold;

    Axis2D axis = graph2d_canvas->canvas.axis;
    float x_axis_len = graph2d_canvas->axis_len.x;
    float y_axis_len = graph2d_canvas->axis_len.y;
    float local_spline_style_control_point_radius = axis2d_scale_into(axis, spline_style->control_point_radius);

    if (begin_tangent_hold) {
        *set_spline_updated = true;
        spline->begin_tangent_normalized = Vector2Normalize(Vector2Subtract(relative_mouse, spline->points[0].coord));
    }
    else if (end_tangent_hold) {
        *set_spline_updated = true;
        spline->end_tangent_normalized = Vector2Normalize(Vector2Subtract(relative_mouse, spline->points[spline->n_points - 1].coord));
    }
    else if (point_hold != -1) { // spline->n_points > 0
        *set_spline_updated = true;
        spline->points[point_hold].coord = relative_mouse;

        float x_low_limit, x_high_limit;
        if (point_hold == 0) {
            x_low_limit = 0;
            x_high_limit = (spline->n_points == 1) ? x_axis_len : spline->points[1].coord.x;
        }
        else if (point_hold == spline->n_points - 1) { // point_hold != 0 && spline->n_points > 1 => spline->n_points > 2
            x_low_limit = spline->points[point_hold - 1].coord.x;
            x_high_limit = x_axis_len;
        }
        else {
            x_low_limit = spline->points[point_hold - 1].coord.x;
            x_high_limit = spline->points[point_hold + 1].coord.x;
        }

        float limit_margin = local_spline_style_control_point_radius;
        Vector2 low_limit = {
            .x = x_low_limit + limit_margin,
            .y = limit_margin,
        };
        Vector2 high_limit = {
            .x = x_high_limit - limit_margin,
            .y = y_axis_len - limit_margin,
        };
        spline->points[point_hold].coord.x = Clamp(spline->points[point_hold].coord.x, low_limit.x, high_limit.x);
        spline->points[point_hold].coord.y = Clamp(spline->points[point_hold].coord.y, low_limit.y, high_limit.y);
    }

    if (*set_spline_updated) {
        spline_calculate_curves(spline);
        *set_spline_updated = false;
    }
}

void spline_entity_draw(SplineEntity* spline_entity) {
    Graph2DCanvas* graph2d_canvas = &spline_entity->graph2d_canvas;
    Spline* spline = &spline_entity->spline;
    SplineStyle* spline_style = &spline_entity->spline_style;
    int point_hold = spline_entity->point_hold;

    graph2d_canvas_draw(graph2d_canvas);
    spline_draw_curves(spline, *spline_style, graph2d_canvas->canvas.axis, point_hold);
}

void draw_point_on_canvas(Axis2D axis, Vector2 pos, float radius) {
    Vector2 pos_in_canvas = axis2d_shift_out(axis, pos);
    DrawCircle(pos_in_canvas.x, pos_in_canvas.y, radius, RED);
    DrawCircle(pos.x, pos.y, radius, BLUE);
}

int main() {

    InitWindow(GLOBAL.SCREEN_WIDTH, GLOBAL.SCREEN_HEIGHT, "CurveMaker");

    // const int SCREEN_MARGIN_X = GLOBAL.SCREEN_WIDTH / 8;
    // const int SCREEN_MARGIN_Y = GLOBAL.SCREEN_HEIGHT / 8;

    Camera2D camera = {0};
    camera.zoom = 1.0;
    // camera.offset = (Vector2) {
    //     .x = SCREEN_MARGIN_X,
    //     .y = GLOBAL.SCREEN_HEIGHT - SCREEN_MARGIN_Y,
    // };

    int spline_count = 4;
    SplineEntity spline_entity_array[4] = {0};

    for (int i = 0; i < spline_count; i++) {
        spline_entity_array[i] = spline_entity_create_default(i);
    }

    float canvas_margin = 50;
    float canvas_margin_half = canvas_margin/2.0;
    float canvas_width = GLOBAL.SCREEN_WIDTH/2.0 - canvas_margin;
    float canvas_height = GLOBAL.SCREEN_HEIGHT/2.0 - canvas_margin;
    graph2d_canvas_set_rect(
        &spline_entity_array[0].graph2d_canvas,
        (Rectangle) {
            .x = canvas_margin_half,
            .y = canvas_margin_half,
            .width = canvas_width,
            .height = canvas_height,
        }
    );
    graph2d_canvas_set_rect(
        &spline_entity_array[1].graph2d_canvas,
        (Rectangle) {
            .x = canvas_margin_half + GLOBAL.SCREEN_WIDTH/2.0,
            .y = canvas_margin_half,
            .width = canvas_width,
            .height = canvas_height,
        }
    );
    graph2d_canvas_set_rect(
        &spline_entity_array[2].graph2d_canvas,
        (Rectangle) {
            .x = canvas_margin_half,
            .y = canvas_margin_half + GLOBAL.SCREEN_HEIGHT/2.0,
            .width = canvas_width,
            .height = canvas_height,
        }
    );
    graph2d_canvas_set_rect(
        &spline_entity_array[3].graph2d_canvas,
        (Rectangle) {
            .x = canvas_margin_half + GLOBAL.SCREEN_WIDTH/2.0,
            .y = canvas_margin_half + GLOBAL.SCREEN_HEIGHT/2.0,
            .width = canvas_width,
            .height = canvas_height,
        }
    );
    

    while (!WindowShouldClose()) {
        
        // INPUT
        for(int i = 0; i < spline_count; i++) {
            spline_entity_set_input(&spline_entity_array[i], camera);
        }
        for(int i = 0; i < spline_count; i++) {
            spline_entity_process_input(&spline_entity_array[i]);
        }

        // UPDATE
        for(int i = 0; i < spline_count; i++) {
            spline_entity_update(&spline_entity_array[i]);
        }
        
        // DRAW
        BeginDrawing();
            ClearBackground(BEIGE);
            BeginMode2D(camera);

                for(int i = 0; i < spline_count; i++) {
                    spline_entity_draw(&spline_entity_array[i]);
                }

            EndMode2D();

            // draw_point_on_canvas(spline_entity.graph2d_canvas.canvas.axis, (Vector2) {50, 50}, 10);

            // const char* n_points_str = TextFormat("Points: %d", spline_entity.spline.n_points);
            // DrawText(n_points_str, 15, 15, 20, RED);

        EndDrawing();
    }
    
    CloseWindow();

    return 0;
}