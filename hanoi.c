/*
 * Towers of Hanoi - Recursive Solution
 * With CLI ASCII diagram and GTK animated visualization
 *
 * Assignment: Chapter 5, Exercise 5.39
 * Uses a recursive function with 4 parameters:
 *   1. Number of disks to move
 *   2. Source peg
 *   3. Destination peg
 *   4. Temporary (auxiliary) peg
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#ifdef USE_GTK
#include <gtk/gtk.h>
#endif

/* ─── Configuration ─────────────────────────────────────────── */
#define MAX_DISKS      8
#define PEG_HEIGHT    (MAX_DISKS + 2)
#define PEG_COUNT      3
#define MAX_MOVES      512
#define DISK_CHAR      '#'
#define PEG_CHAR       '|'

/* Width of the widest disk (must be odd) */
#define MAX_DISK_WIDTH (MAX_DISKS * 2 + 1)
/* Total width of one peg column in the diagram */
#define COL_WIDTH      (MAX_DISK_WIDTH + 4)

/* ─── Data Structures ────────────────────────────────────────── */
typedef struct {
    int disks[MAX_DISKS]; /* disk sizes stored bottom→top */
    int top;              /* index of next free slot      */
} Peg;

typedef struct {
    int from;
    int to;
} Move;

static Peg  pegs[PEG_COUNT];
static Move moves[MAX_MOVES];
static int  move_count = 0;
static int  num_disks  = 3;

/* ─── Peg helpers ────────────────────────────────────────────── */
static void peg_push(Peg *p, int disk) { p->disks[p->top++] = disk; }
static int  peg_pop (Peg *p)           { return p->disks[--p->top]; }

static void init_pegs(int n)
{
    memset(pegs, 0, sizeof(pegs));
    for (int i = n; i >= 1; i--)
        peg_push(&pegs[0], i); /* largest disk at bottom */
}

/* ─── ASCII diagram ──────────────────────────────────────────── */
static void print_diagram(int step, int from, int to)
{
    if (step == 0)
        printf("\n  Initial state:\n");
    else
        printf("\n  Step %d:  Peg %d  -->  Peg %d\n", step, from + 1, to + 1);

    /* Print rows from top to bottom */
    for (int row = PEG_HEIGHT - 1; row >= 0; row--) {
        for (int p = 0; p < PEG_COUNT; p++) {
            /* How many disks does this peg have? */
            int disk_at_row = -1;
            if (row < pegs[p].top)
                disk_at_row = pegs[p].disks[row];

            /* Center position within the column */
            int half = COL_WIDTH / 2;

            if (row == 0) {
                /* Base row */
                for (int c = 0; c < COL_WIDTH; c++) putchar('=');
            } else if (disk_at_row > 0) {
                /* Draw disk */
                int disk_half = disk_at_row; /* half-width of disk */
                for (int c = 0; c < COL_WIDTH; c++) {
                    int dist = abs(c - half);
                    putchar(dist <= disk_half ? DISK_CHAR : ' ');
                }
            } else {
                /* Empty row - just the peg pole */
                for (int c = 0; c < COL_WIDTH; c++)
                    putchar(c == half ? PEG_CHAR : ' ');
            }
        }
        putchar('\n');
    }

    /* Peg labels */
    for (int p = 0; p < PEG_COUNT; p++) {
        int half = COL_WIDTH / 2;
        for (int c = 0; c < COL_WIDTH; c++)
            putchar(c == half ? ('1' + p) : ' ');
    }
    putchar('\n');
}

/* ─── Recursive solver ───────────────────────────────────────── */
static void hanoi(int n, int src, int dst, int tmp)
{
    if (n == 0) return;

    /* Move n-1 disks from src to tmp, using dst as auxiliary */
    hanoi(n - 1, src, tmp, dst);

    /* Move the largest disk from src to dst */
    int disk = peg_pop(&pegs[src]);
    peg_push(&pegs[dst], disk);

    /* Record and display */
    if (move_count < MAX_MOVES) {
        moves[move_count].from = src;
        moves[move_count].to   = dst;
    }
    move_count++;

    printf("  Move %d:  Peg %d --> Peg %d\n", move_count, src + 1, dst + 1);
    print_diagram(move_count, src, dst);

    /* Brief pause so the output is readable when scrolling */
    struct timespec ts = {0, 200000000L}; /* 0.2 s */
    nanosleep(&ts, NULL);

    /* Move n-1 disks from tmp to dst, using src as auxiliary */
    hanoi(n - 1, tmp, dst, src);
}

/* ─── CLI entry point ────────────────────────────────────────── */
static void run_cli(void)
{
    printf("\n");
    printf("  ╔══════════════════════════════════════════╗\n");
    printf("  ║       TOWERS OF HANOI  — %d disks        ║\n", num_disks);
    printf("  ║  Move all disks from Peg 1 to Peg 3     ║\n");
    printf("  ║  Rules:                                  ║\n");
    printf("  ║   • Move only one disk at a time         ║\n");
    printf("  ║   • Never place a larger disk on top     ║\n");
    printf("  ╚══════════════════════════════════════════╝\n");

    init_pegs(num_disks);
    print_diagram(0, -1, -1);

    printf("\n  ── Move sequence ──────────────────────────\n");
    hanoi(num_disks, 0, 2, 1); /* src=1, dst=3, tmp=2 */

    printf("\n  ── Summary ────────────────────────────────\n");
    printf("  Total moves : %d\n", move_count);
    printf("  Formula     : 2^%d - 1 = %d\n", num_disks, (1 << num_disks) - 1);
    printf("  All disks are now on Peg 3.\n\n");
}

/* ══════════════════════════════════════════════════════════════ */
/*                          GTK SECTION                          */
/* ══════════════════════════════════════════════════════════════ */
#ifdef USE_GTK

/* Colors for disks 1..MAX_DISKS */
static const double DISK_COLORS[][3] = {
    {0.95, 0.26, 0.21}, /* red       */
    {1.00, 0.60, 0.00}, /* orange    */
    {1.00, 0.92, 0.23}, /* yellow    */
    {0.30, 0.69, 0.31}, /* green     */
    {0.13, 0.59, 0.95}, /* blue      */
    {0.61, 0.15, 0.69}, /* purple    */
    {0.00, 0.74, 0.83}, /* cyan      */
    {0.76, 0.09, 0.36}, /* pink      */
};

/* GTK state */
typedef struct {
    GtkWidget  *drawing_area;
    GtkWidget  *label_step;
    GtkWidget  *label_move;
    GtkWidget  *btn_next;
    GtkWidget  *btn_prev;
    GtkWidget  *btn_auto;
    GtkWidget  *speed_scale;
    Peg         peg_state[MAX_MOVES + 1][PEG_COUNT];
    int         current_step;  /* 0 = initial, 1..move_count = after each move */
    guint       timer_id;
    gboolean    playing;
} AppState;

static AppState app;

/* Reconstruct peg states for every step */
static void build_peg_states(void)
{
    init_pegs(num_disks);
    memcpy(app.peg_state[0], pegs, sizeof(pegs));

    for (int m = 0; m < move_count; m++) {
        int disk = peg_pop(&pegs[moves[m].from]);
        peg_push(&pegs[moves[m].to], disk);
        memcpy(app.peg_state[m + 1], pegs, sizeof(pegs));
    }
}

static gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    (void)data;

    int W = gtk_widget_get_allocated_width(widget);
    int H = gtk_widget_get_allocated_height(widget);

    /* Background */
    cairo_set_source_rgb(cr, 0.12, 0.12, 0.18);
    cairo_paint(cr);

    int step = app.current_step;
    Peg *ps   = app.peg_state[step];

    /* Layout */
    int base_y      = H - 60;
    int base_h      = 18;
    int peg_w       = 10;
    int peg_h       = H - 120;
    int disk_h      = 22;
    int max_disk_w  = (W / PEG_COUNT) - 40;
    int peg_spacing = W / PEG_COUNT;

    /* Draw base */
    cairo_set_source_rgb(cr, 0.55, 0.35, 0.18);
    cairo_rectangle(cr, 20, base_y, W - 40, base_h);
    cairo_fill(cr);

    for (int p = 0; p < PEG_COUNT; p++) {
        int cx = peg_spacing * p + peg_spacing / 2;

        /* Peg pole */
        cairo_set_source_rgb(cr, 0.70, 0.50, 0.25);
        cairo_rectangle(cr, cx - peg_w / 2, base_y - peg_h, peg_w, peg_h);
        cairo_fill(cr);

        /* Peg label */
        cairo_set_source_rgb(cr, 0.85, 0.85, 0.85);
        cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 20);
        char label[4];
        snprintf(label, sizeof(label), "P%d", p + 1);
        cairo_text_extents_t te;
        cairo_text_extents(cr, label, &te);
        cairo_move_to(cr, cx - te.width / 2, base_y + base_h + 28);
        cairo_show_text(cr, label);

        /* Disks */
        for (int d = 0; d < ps[p].top; d++) {
            int disk    = ps[p].disks[d];
            double frac = (double)disk / num_disks;
            int dw      = (int)(max_disk_w * frac * 0.9 + max_disk_w * 0.1);
            int dy      = base_y - disk_h * (d + 1) - 2;

            /* Color */
            int ci = (disk - 1) % 8;
            cairo_set_source_rgb(cr, DISK_COLORS[ci][0],
                                     DISK_COLORS[ci][1],
                                     DISK_COLORS[ci][2]);

            /* Rounded rectangle */
            double x  = cx - dw / 2.0;
            double y  = dy;
            double r  = 6.0;
            cairo_new_sub_path(cr);
            cairo_arc(cr, x + dw - r, y + r, r, -G_PI_2, 0);
            cairo_arc(cr, x + dw - r, y + disk_h - r, r, 0, G_PI_2);
            cairo_arc(cr, x + r, y + disk_h - r, r, G_PI_2, G_PI);
            cairo_arc(cr, x + r, y + r, r, G_PI, -G_PI_2);
            cairo_close_path(cr);
            cairo_fill_preserve(cr);

            /* Border */
            cairo_set_source_rgba(cr, 0, 0, 0, 0.4);
            cairo_set_line_width(cr, 1.5);
            cairo_stroke(cr);

            /* Disk number */
            cairo_set_source_rgb(cr, 1, 1, 1);
            cairo_set_font_size(cr, 13);
            char dn[4];
            snprintf(dn, sizeof(dn), "%d", disk);
            cairo_text_extents(cr, dn, &te);
            cairo_move_to(cr, cx - te.width / 2, y + disk_h / 2.0 + te.height / 2.0);
            cairo_show_text(cr, dn);
        }
    }

    return FALSE;
}

static void update_labels(void)
{
    char buf[128];
    if (app.current_step == 0) {
        gtk_label_set_text(GTK_LABEL(app.label_step), "Step 0 — Initial state");
        gtk_label_set_text(GTK_LABEL(app.label_move), "");
    } else {
        int m = app.current_step - 1;
        snprintf(buf, sizeof(buf), "Step %d of %d", app.current_step, move_count);
        gtk_label_set_text(GTK_LABEL(app.label_step), buf);
        snprintf(buf, sizeof(buf), "Peg %d  →  Peg %d",
                 moves[m].from + 1, moves[m].to + 1);
        gtk_label_set_text(GTK_LABEL(app.label_move), buf);
    }
    gtk_widget_set_sensitive(app.btn_prev, app.current_step > 0);
    gtk_widget_set_sensitive(app.btn_next, app.current_step < move_count);
}

static void redraw(void)
{
    update_labels();
    gtk_widget_queue_draw(app.drawing_area);
}

static gboolean auto_advance(gpointer data)
{
    (void)data;
    if (app.current_step < move_count) {
        app.current_step++;
        redraw();
        return TRUE;
    }
    app.playing = FALSE;
    gtk_button_set_label(GTK_BUTTON(app.btn_auto), "▶  Play");
    app.timer_id = 0;
    return FALSE;
}

static void on_next(GtkWidget *w, gpointer d)
{
    (void)w; (void)d;
    if (app.current_step < move_count) { app.current_step++; redraw(); }
}

static void on_prev(GtkWidget *w, gpointer d)
{
    (void)w; (void)d;
    if (app.current_step > 0) { app.current_step--; redraw(); }
}

static void on_play(GtkWidget *w, gpointer d)
{
    (void)d;
    if (app.playing) {
        /* Pause */
        if (app.timer_id) { g_source_remove(app.timer_id); app.timer_id = 0; }
        app.playing = FALSE;
        gtk_button_set_label(GTK_BUTTON(w), "▶  Play");
    } else {
        if (app.current_step >= move_count) app.current_step = 0;
        app.playing = TRUE;
        gtk_button_set_label(GTK_BUTTON(w), "⏸  Pause");
        double speed = gtk_range_get_value(GTK_RANGE(app.speed_scale));
        guint interval = (guint)(1000.0 / speed); /* ms per step */
        app.timer_id = g_timeout_add(interval, auto_advance, NULL);
    }
}

static void on_reset(GtkWidget *w, gpointer d)
{
    (void)w; (void)d;
    if (app.timer_id) { g_source_remove(app.timer_id); app.timer_id = 0; }
    app.playing = FALSE;
    gtk_button_set_label(GTK_BUTTON(app.btn_auto), "▶  Play");
    app.current_step = 0;
    redraw();
}

static void run_gtk(int argc, char **argv)
{
    gtk_init(&argc, &argv);

    build_peg_states();

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Towers of Hanoi — Animated");
    gtk_window_set_default_size(GTK_WINDOW(window), 720, 520);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    /* Apply dark CSS */
    GtkCssProvider *css = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css,
        "window { background-color: #1e1e2e; }"
        "label  { color: #cdd6f4; font-size: 15px; }"
        "#step-label { font-size: 17px; font-weight: bold; color: #89b4fa; }"
        "#move-label { font-size: 22px; font-weight: bold; color: #a6e3a1; }"
        "button { background: #313244; color: #cdd6f4; border-radius: 8px;"
        "         padding: 6px 16px; border: 1px solid #45475a; font-size: 14px; }"
        "button:hover { background: #45475a; }", -1, NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(css),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    /* Layout */
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 12);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    /* Title */
    GtkWidget *title = gtk_label_new(NULL);
    char title_buf[128];
    snprintf(title_buf, sizeof(title_buf),
             "<span size='x-large' weight='bold' color='#cba6f7'>"
             "Towers of Hanoi  (%d disks)</span>", num_disks);
    gtk_label_set_markup(GTK_LABEL(title), title_buf);
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 4);

    /* Step label */
    app.label_step = gtk_label_new("Step 0 — Initial state");
    gtk_widget_set_name(app.label_step, "step-label");
    gtk_box_pack_start(GTK_BOX(vbox), app.label_step, FALSE, FALSE, 0);

    /* Move label */
    app.label_move = gtk_label_new("");
    gtk_widget_set_name(app.label_move, "move-label");
    gtk_box_pack_start(GTK_BOX(vbox), app.label_move, FALSE, FALSE, 0);

    /* Drawing area */
    app.drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(app.drawing_area, 700, 340);
    g_signal_connect(app.drawing_area, "draw", G_CALLBACK(on_draw), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), app.drawing_area, TRUE, TRUE, 4);

    /* Controls row */
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_set_homogeneous(GTK_BOX(hbox), FALSE);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 4);

    GtkWidget *btn_reset = gtk_button_new_with_label("⟳  Reset");
    g_signal_connect(btn_reset, "clicked", G_CALLBACK(on_reset), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), btn_reset, FALSE, FALSE, 0);

    app.btn_prev = gtk_button_new_with_label("◀  Prev");
    g_signal_connect(app.btn_prev, "clicked", G_CALLBACK(on_prev), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), app.btn_prev, FALSE, FALSE, 0);

    app.btn_next = gtk_button_new_with_label("Next  ▶");
    g_signal_connect(app.btn_next, "clicked", G_CALLBACK(on_next), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), app.btn_next, FALSE, FALSE, 0);

    app.btn_auto = gtk_button_new_with_label("▶  Play");
    g_signal_connect(app.btn_auto, "clicked", G_CALLBACK(on_play), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), app.btn_auto, FALSE, FALSE, 0);

    /* Speed */
    GtkWidget *speed_lbl = gtk_label_new("Speed:");
    gtk_box_pack_start(GTK_BOX(hbox), speed_lbl, FALSE, FALSE, 8);
    app.speed_scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.5, 5.0, 0.5);
    gtk_range_set_value(GTK_RANGE(app.speed_scale), 1.5);
    gtk_widget_set_size_request(app.speed_scale, 140, -1);
    gtk_box_pack_start(GTK_BOX(hbox), app.speed_scale, FALSE, FALSE, 0);

    /* Total moves info */
    char info[64];
    snprintf(info, sizeof(info), "Total moves: %d  (2^%d − 1)", move_count, num_disks);
    GtkWidget *info_lbl = gtk_label_new(info);
    gtk_box_pack_end(GTK_BOX(hbox), info_lbl, FALSE, FALSE, 8);

    update_labels();
    gtk_widget_show_all(window);
    gtk_main();
}
#endif /* USE_GTK */

/* ─── main ───────────────────────────────────────────────────── */
int main(int argc, char *argv[])
{
    if (argc >= 2) {
        num_disks = atoi(argv[1]);
        if (num_disks < 1 || num_disks > MAX_DISKS) {
            fprintf(stderr, "Usage: %s [disks 1-%d]\n", argv[0], MAX_DISKS);
            return 1;
        }
    } else {
        num_disks = 3; /* default */
    }

    /* Always run the CLI version first */
    run_cli();

#ifdef USE_GTK
    printf("  Launching GTK window…\n\n");
    run_gtk(argc, argv);
#else
    printf("  (Build with 'make gtk' to launch the animated GTK window.)\n\n");
#endif

    return 0;
}
