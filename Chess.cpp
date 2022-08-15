#include <FL/Fl.H> 
#include <FL/Fl_Window.H> 
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#include <iostream>
#include <string>

using namespace std;

const int MAX_MOVES = 2*100;
const int WINDOW_SIZE_X = 1500;
const int WINDOW_SIZE_Y = 800;
const int BOARD_WHITE =  17;
const int BOARD_BLACK =  100;
const int PIECES_WHITE = 7;
const int PIECES_BLACK = 0;
const int PIECE_BOUNDARY = 0; 
const int BACKGROUND = 30; 


/* Each piece has a code : w pawn = 1, w bishop = 2, w knight = 3, w rook = 4, w queen = 5, w king = 6
black piece has a code = 6+white */


/* INITIAL POSITION IN CHESS*/

void default_IP(int* default_initial_position){
    for (int i = 8; i < 16; i++){
        default_initial_position[i] = 1;
        default_initial_position[i+40] = 7;
    }
    default_initial_position[0] = default_initial_position[7] = 4;
    default_initial_position[1] = default_initial_position[6] = 3;
    default_initial_position[2] = default_initial_position[5] = 2;
    default_initial_position[3] = 5; default_initial_position[4] = 6;
    default_initial_position[56] = default_initial_position[63] = 10;
    default_initial_position[57] = default_initial_position[62] = 9;
    default_initial_position[58] = default_initial_position[61] = 8;
    default_initial_position[59] = 11; default_initial_position[60] = 12;
    for (int i = 16; i < 48; i++){
        default_initial_position[i] = 0;
    }
}

/*------------------------------------------------------------*/


class Mapping{
    int x_m, y_m, w_m, h_m, Number_of_Rows, Number_of_Columns;
public:
    Mapping(int w = 80, int h = 80, int x = 80, int y = WINDOW_SIZE_Y-160, int NR = 8, int NC = 8){
        x_m = x;
        y_m = y;
        w_m = w;
        h_m = h;
        Number_of_Rows = NR;
        Number_of_Columns = NC;
    }
    int get_square(double x, double y){
        if (x > x_m+Number_of_Columns*w_m || x < x_m || y > y_m+h_m || y < y_m-(Number_of_Rows-1)*h_m) {
            return 100;
        }
        double X = x-x_m, Y = y_m+h_m-y;
        int f = X/w_m;
        int r = Y/h_m;
        return get_i(r, f);
    }
    int get_button(double x, double y){
        if (get_square(x,y) != 100){
            return 0;
        }
        else if (x < 770 && x > 735 && y < 410 && y > 390){
            return 1;
        }
        else if(x > 1100 && x < 1270 && y > 100 && y < 150){
            return 2;
        }
        else if(x < 780 && x > 750 && y > 100 && y < 120){
            return 3;
        }
        else if(x < 780 && x > 750 && y > 680 && y < 720){
            return 4;
        }
        else if(x > 780 && x < 840 && y > 100 && y < 120){
            return 5;
        }
        else if(x > 780 && x < 840 && y > 680 && y < 720){
            return 6;
        }
        else if(x > 1100 && x < 1150 & y > 600 && y < 650){
            return 7;
        }
        else if(x > 1200 && x < 1250 & y > 600 && y < 650){
            return 8;
        }
        else if(x > 1150 && x < 1200 & y > 650 && y < 700){
            return 9;
        }
        return -1;
    }
    int rank(int p){
        return p/Number_of_Columns;
    }
    int file(int p){
        return p%Number_of_Columns;
    }
    int get_i(int rank, int file){
        return Number_of_Columns*rank + file;
    }
    int get_xm (int p){
        int f = file(p);
        return x_m+f*w_m;
    }  
    int get_ym (int p){
        int r = rank(p);
        return y_m-r*h_m;
    }
    int get_wm(int p){
        return w_m;
    }
    int get_hm(int p){
        return h_m;
    }
};

class Memory{
    int moves[64*MAX_MOVES];
public:
    Memory(){};
    void add_move(int* c, int n){
        for (int i = 0; i < 64; i++){
            moves[i+n*64] = c[i];
        }
    }
    void fetch_move(int *c, int n){
        for (int i = 0; i < 64; i++){
            c[i] = moves[n*64+i];
        }
    }
};

class CU : public Fl_Widget, public Mapping, public Memory {
protected:
    bool whites_move;
    int previous_position[64];
    int current_position[64];
    int piece_white, piece_black, board_white, board_black;
    int prev_select, prev_drop;
    int select, drop;
    int move_number;
    int display_move;
public:
    CU(bool white_to_move = 1, int dim_x = WINDOW_SIZE_X, int dim_y = WINDOW_SIZE_Y, int size = 80, int board_white = BOARD_WHITE, 
    int board_black = BOARD_BLACK, int piece_white = PIECES_WHITE, int piece_black = PIECES_BLACK) : 
    Mapping(size, size), Fl_Widget(0, 0, dim_x, dim_y) , Memory(){
        this->select = 100;
        this->drop = 100;
        this->move_number = 0;
        this->display_move = -1;
        this->whites_move = white_to_move;
        this->board_white = board_white;
        this->board_black = board_black;
        this->piece_white = piece_white;
        this->piece_black = piece_black;
        default_IP(this->current_position);
        default_IP(this->previous_position);
    }
    bool is_white(int code){
        if (7 <= code && code <= 12){
            return 0;
        }
        else if (1 <= code && code <= 6){
            return 1;
        }
    }
    bool check_validity(int i){
        int j = current_position[i];
        int k = current_position[select];
        if (((j <= 12 && j >= 7)&&(k >=7 && k <= 12))||((j>0 && j <=6)&&(k>0&&k<=6))){
            return 0;
        }
        return 1;
    }

    void update_position(){
        int ID = this->current_position[select];
        this->previous_position[prev_select] = this->current_position[prev_select];
        this->previous_position[prev_drop] = this->current_position[prev_drop];
        this->current_position[select] = 0;
        this->current_position[drop] = ID;
        add_move(current_position, move_number);
    }
    void next_move(){
        prev_select = select;
        prev_drop = drop;
        drop = 100;
        select = 100;
        whites_move = !whites_move;
        move_number += 1;
        display_move += 1;
    }
    void new_game(){
        default_IP(this->current_position);
        default_IP(this->previous_position);
        this->whites_move = 1;
        this->move_number = 0;
        this->display_move = -1;
    }

    void draw_piece(int i, int id);     // line number : 490
    void pawn(int i, bool white);       // line number : 512
    void bishop(int i, bool white);     // line number : 534
    void knight(int i, bool white);     // line number : 569
    void rook(int i, bool white);       // line number : 619
    void queen(int i, bool white);      // line number : 678
    void king(int i, bool white);       // line number : 738
};

class ChessBoard : public CU {
protected:
    int dim_x, dim_y, size; 
    bool flipped; int complete_redraw;
    bool freeze, game_end;
    bool same_colour;
public:
    ChessBoard(bool flip = 0, int size = 80, int dim_x = 640, int dim_y = 640) : CU() {
        this->flipped = flip;
        this->complete_redraw = -100;
        this->freeze = 0;
        this->game_end = 0;
        this->dim_x = dim_x;
        this->dim_y = dim_y;
        this->size = size;
    }
    int tile_colour(int i){
        if ((i+i/8)%2 == 1){
            return board_black;
        }
        return board_white;
    }
    void draw_tile(int i){
        int j = i;
        if (flipped){
            j = 63-i;
        }
        fl_draw_box (FL_FLAT_BOX, get_xm(j), get_ym(j), 80, 80, tile_colour(j));
        draw_piece(j, current_position[i]);
    }
    void draw_coord(){
        fl_color(7);
        fl_font(FL_TIMES, 15);
        for (int i = 0; i < 8; i++){
            int j = i;
            if (flipped){
                j = 7-i;
            }
            char C[2]; C[1] = '\0';
            C[0] = 'a'+j;
            fl_draw(C, get_xm(i)+get_wm(i)/2, get_ym(i)+5*get_hm(i)/4);
            C[0] = '1'+j;
            fl_draw(C, get_xm(i*8)-get_wm(i)/4, get_ym(i*8)+get_hm(i)/2);
        }
    }
    void draw_board(){
        if (complete_redraw != 0){
            select = 100;
            for (int i = 0; i < 64; i++){
                draw_tile(i);
            }
            draw_coord();
        }
        else if (select != 100 && same_colour){
            highlight(select, 0);
            select = drop;
            drop = 100;
            highlight(select);
            same_colour = 0;
        }
        else if (drop != 100){
            highlight(prev_select, 0);
            highlight(prev_drop, 0);
            highlight(select);
            highlight(drop);
            next_move();
        }
        
        else if (select != 100){
            highlight(select);
        }

        this->complete_redraw = 0;
    }
    void draw_interface();          // line number : 424

    void draw(){
        draw_interface();
        draw_board();
    }
    int handle(int e){
        double x = Fl::event_x();
        double y = Fl::event_y();
        int i = get_square(x, y); 
        int j = get_button(x, y);
        if (e == FL_PUSH && i < 64 && i >= 0){
            handle_moves(i);
        }
        else if(e == FL_PUSH && j != 0){
            handle_interface(j);
        }
        redraw();
        return 1;
    }
    void handle_interface(int i){
        if (i == 1){
            flipped = !flipped;
            complete_redraw = -1;
        }
        else if (i == 2){
            complete_redraw = -100;
            freeze = 0;
            game_end = 0;
            new_game();
        }
        else if (i <= 6 && i >= 3){
            if (freeze){
                return;
            }
            freeze = 1;
            game_end = 1;
            if (i == 3 || i == 4){
                complete_redraw = -2;
            }
            if (i == 5 || i == 6){
                if (flipped && i == 6){
                    complete_redraw = -3;
                }
                else if (flipped && i == 5){
                    complete_redraw = -4;
                }
                else if (i == 5){
                    complete_redraw = -3;
                }
                else if (i == 6){
                    complete_redraw = -4;
                }
            }
        }
        else if (i == 7){
            if (display_move > 0) {
                display_move -= 1;
                fetch_move(current_position, display_move);
                complete_redraw = -1;
                freeze = 1;
            }
        }
        else if (i == 8){
            if (display_move < move_number-1){
                fetch_move(current_position, display_move+1);
                complete_redraw = -1;
                display_move += 1;
                if (display_move == move_number-1 && !game_end){
                    freeze = 0;
                }
            }
        }
        else if (i == 9){
            if (move_number > 0){
                fetch_move(current_position, move_number-1);
                display_move = move_number-1;
                if (!game_end){
                    freeze = 0;
                }
                complete_redraw = -1;
            }  
        }
    }
    void handle_moves(int i){
        if (freeze){
            return;
        }
        if (flipped && i < 64 && i >= 0){
            i = 63-i;
        }
        if (select == 100 && (is_white(current_position[i]) == whites_move && current_position[i] != 0)){
            select = i;
        }
        else if( select < 64 && select >= 0 && check_validity(i) && i != select){
            drop = i;
            update_position();
        }
        else if( select < 64 && select >= 0 && !check_validity(i) && i != select){
            drop = i;
            same_colour = 1;
        }
    }
    void highlight(int i, bool highlight = 1){
        int j = i;
        if (flipped){
            j = 63-i;
        }
        if (highlight){
            fl_draw_box (FL_FLAT_BOX, get_xm(j), get_ym(j), 80, 80, 175);
            draw_piece(j, current_position[i]);
            return;
        }
        draw_tile(i);
    }
};
 /* _____________________MAIN PROGRAM _________________________*/
int main(){
    int a[2][3];
    a[0][0] = 1;
    cout << a[0][0];
    Fl_Window window(WINDOW_SIZE_X,WINDOW_SIZE_Y,"Chess.com"); // outer window
	ChessBoard c;
	window.color(BACKGROUND);
    window.end();
    window.show();
return(Fl::run());  
}


/* ______________Draw Sub functions______________________*/

void ChessBoard::draw_interface(){
    if (complete_redraw == -1){
        fl_draw_box(FL_FLAT_BOX, 0, 0, 80, 800, BACKGROUND);
        fl_draw_box(FL_FLAT_BOX, 0, 720, 720, 80, BACKGROUND);
    }
    else if (complete_redraw == -100){
        fl_draw_box(FL_FLAT_BOX, 0, 0, 80, 800, BACKGROUND);
        fl_draw_box(FL_FLAT_BOX, 0, 720, 720, 80, BACKGROUND);
        fl_draw_box(FL_FLAT_BOX, 1100, 190, 200, 50, BACKGROUND);
        // flip button
        fl_draw_box (FL_FLAT_BOX, 735, 390, 35, 20, 136);
        fl_color(7);
        fl_font(FL_TIMES,15);
        fl_draw("flip", 740, 405);
        // New Game button
        fl_draw_box (FL_FLAT_BOX, 1100, 100, 170, 50, 131);
        fl_color(0);
        fl_font(FL_TIMES_BOLD,30);
        fl_draw("New Game", 1110, 130);
        // White vs Black
        fl_color(7);
        fl_font(FL_TIMES_BOLD,20);
        fl_draw("White vs Black", 1110, 180);
        // resign/draw buttons
        fl_draw_box (FL_FLAT_BOX, 750, 100, 30, 20, 239);
        fl_draw_box (FL_FLAT_BOX, 750, 680, 30, 20, 239);
        fl_draw_box (FL_FLAT_BOX, 780, 100, 60, 20, 252);
        fl_draw_box (FL_FLAT_BOX, 780, 680, 60, 20, 252);
        fl_color(0);
        fl_font(FL_TIMES, 15);
        fl_draw("1/2" ,750, 115);
        fl_draw("1/2" ,750, 695);
        fl_draw("Resign" ,785, 115);
        fl_draw("Resign" ,785, 695);
        // Move Navigation
        fl_draw_box (FL_FLAT_BOX, 1100, 600, 50, 50, 239);
        fl_draw_box (FL_FLAT_BOX, 1200, 600, 50, 50, 239);
        fl_draw_box (FL_FLAT_BOX, 1150, 650, 50, 50, 239);
        fl_color(0);
        fl_font(FL_TIMES, 40);
        fl_draw("<" , 1110, 640);
        fl_draw(">" , 1215, 640);
        fl_draw("^" , 1165, 690);

    }
    else if (complete_redraw == -2){
        fl_color(7);
        fl_font(FL_TIMES_BOLD, 20);
        fl_draw("    1/2    -    1/2  ", 1110, 210);
    }
    else if (complete_redraw == -3){
        fl_color(7);
        fl_font(FL_TIMES_BOLD,20);
        fl_draw("      1    -    0  ", 1110, 210);
    }
    else if (complete_redraw == -4){
        fl_color(7);
        fl_font(FL_TIMES_BOLD,20);
        fl_draw("      0    -    1  ", 1110, 210);
    }
}

/* ______________PIECE ARTS______________________________*/
/* this just brute force code*/

void CU::draw_piece(int i, int id){
    if (id == 1 || id == 7){
        pawn(i, is_white(id));
    }
    else if (id == 2 || id == 8){
        bishop(i, is_white(id));
    }
    else if (id == 3 || id == 9){
        knight(i, is_white(id));
    }
    else if (id == 4 || id == 10){
        rook(i, is_white(id));
    }
    else if (id == 5 || id == 11){
        queen(i, is_white(id));
    }
    else if (id == 6 || id == 12){
        king(i, is_white(id));
    }
    return;
}

void CU::pawn(int i, bool white){
    int x = get_xm(i), y = get_ym(i), w = get_wm(i), h = get_hm(i);
    int c = piece_white, c1 = piece_black;
    if (!white){
        c = piece_black;
        c1 = piece_white;
    }
    fl_color(c);
    fl_pie(x+3*w/16, y+h/2, 5*w/8, 7*h/8, 0, 180);
    fl_pie(x+5*w/16, y+5*h/16, 3*w/8, 3*h/8, 0, 360);
    fl_pie(x+3*w/8, y+h/8, w/4, h/4, 0, 360);
    if (white){
        fl_color(c1);
        fl_line_style(0, 2);
        fl_line(x+3*w/16, y+15*w/16, x+13*w/16, y+15*w/16);
        fl_arc(x+3*w/16, y+h/2, 5*w/8, 7*h/8, 0, 58);
        fl_arc(x+3*w/16, y+h/2, 5*w/8, 7*h/8, 122, 180);
        fl_arc(x+5*w/16, y+5*h/16, 3*w/8, 3*h/8, -30, 60);
        fl_arc(x+5*w/16, y+5*h/16, 3*w/8, 3*h/8, 120, 210);
        fl_arc(x+3*w/8, y+h/8, w/4, h/4, -60, 240);
    }
}
void CU::bishop(int i, bool white){
    int x = get_xm(i), y = get_ym(i), w = get_wm(i), h = get_hm(i);
    int c = piece_white, c1 = piece_black;
    w *= 1.1; h*= 1.1;
    y -= 3*h/32; x -= w/16;
    if (!white){
        c = piece_black;
        c1 = piece_white;
    }
    fl_color(c);
    fl_pie(x+7*w/16, y+h/8+3, w/8, h/8, 0, 360);
    fl_begin_complex_polygon();
    fl_curve(x+w/2, y+w/4, x, y+4*h/5, x+w, y+4*h/5, x+w/2, y+w/4);
    fl_end_complex_polygon();
    fl_polygon(x+7*w/16, y+5*h/8, x+9*w/16, y+5*h/8, x+9*w/16, y+7*h/8, x+7*w/16, y+7*h/8);
    fl_polygon(x+4*w/16, y+15*h/16, x+12*w/16, y+15*h/16, x+12*w/16, y+7*h/8, x+4*w/16, y+7*h/8);
    fl_color(c1);
    fl_line_style(0,2);
    fl_line(x+7*w/16, y+4*h/8, x+9*w/16, y+4*h/8);
    fl_line(x+8*w/16, y+7*h/16, x+8*w/16, y+9*h/16);
    fl_line_style(0, 2);
    if (white){
        fl_begin_loop();
        fl_curve(x+w/2, y+w/4+2, x, y+4*h/5, x+w, y+4*h/5, x+w/2, y+w/4+2);
        fl_end_loop();
        fl_arc(x+7*w/16, y+h/8+3, w/8, h/8, 0, 360);
        fl_loop(x+7*w/16, y+11*h/16, x+9*w/16, y+11*h/16, x+9*w/16, y+7*h/8, x+7*w/16, y+7*h/8);
        fl_loop(x+4*w/16, y+15*h/16, x+12*w/16, y+15*h/16, x+12*w/16, y+7*h/8, x+4*w/16, y+7*h/8);
    }
    else{
        fl_line(x+7*w/16, y+11*h/16, x+9*w/16, y+11*h/16);
        fl_line(x+9*w/16, y+7*h/8, x+7*w/16, y+7*h/8);
    }
}

void CU::knight(int i, bool white){
    int x = get_xm(i), y = get_ym(i), w = get_wm(i), h = get_hm(i);
    int c = piece_white, c1 = piece_black;
    x -= w/16;
    if (!white){
        c = piece_black;
        c1 = piece_white;
    }
    fl_color(c);
    fl_begin_complex_polygon();
    fl_vertex(x+7*w/8, y+6*h/8);
    fl_vertex(x+5*w/8, y+h/4);
    fl_vertex(x+9*w/16, y+3*h/16);
    fl_vertex(x+h/2, y+h/4);
    fl_vertex(x+w/8, y+3*h/8);
    fl_vertex(x+w/8+w/24, y+3*h/8+h/12);
    fl_vertex(x+w/8+w/24+w/16, y+3*h/8+h/12-h/32);
    fl_vertex(x+w/8+w/24+0.5, y+3*h/8+h/12+1);
    fl_vertex(x+w/8+w/16+0.5, y+3*h/8+h/8+1);
    fl_vertex(x+4*w/8, y+7*h/16);
    fl_vertex(x+4*w/8, y+6*h/8);
    fl_vertex(x+3*w/8, y+6*h/8);
    fl_vertex(x+3*w/8, y+7*h/8);
    fl_vertex(x+15*w/16, y+7*h/8);
    fl_vertex(x+15*w/16, y+6*h/8);
    fl_end_complex_polygon();
    fl_color(c1);
    fl_line_style(0, 2);
    fl_arc(x+w/4+w/8+w/16, y+h/4+h/16, w/16, h/16, 0, 360);
    if (white){
        fl_line(x+4*w/8, y+6*h/8,x+15*w/16, y+6*h/8);
        fl_begin_loop();
        fl_vertex(x+7*w/8, y+6*h/8);
        fl_vertex(x+5*w/8, y+h/4);
        fl_vertex(x+9*w/16, y+3*h/16);
        fl_vertex(x+h/2, y+h/4);
        fl_vertex(x+w/8, y+3*h/8);
        fl_vertex(x+w/8+w/24, y+3*h/8+h/12);
        fl_vertex(x+w/8+w/24+w/16, y+3*h/8+h/12-h/32);
        fl_vertex(x+w/8+w/24+0.5, y+3*h/8+h/12+1);
        fl_vertex(x+w/8+w/16+0.5, y+3*h/8+h/8+1);
        fl_vertex(x+4*w/8, y+7*h/16);
        fl_vertex(x+4*w/8, y+6*h/8);
        fl_vertex(x+3*w/8, y+6*h/8);
        fl_vertex(x+3*w/8, y+7*h/8);
        fl_vertex(x+15*w/16, y+7*h/8);
        fl_vertex(x+15*w/16, y+6*h/8);
        fl_end_loop();
    }
}
void CU::rook(int i, bool white){
    int x = get_xm(i), y = get_ym(i), w = get_wm(i), h = get_hm(i);
    int c = piece_white, c1 = piece_black;
    y += h/16;
    if (!white){
        c = piece_black;
        c1 = piece_white;
    }
    fl_color(c);
    fl_begin_complex_polygon();
    fl_vertex(x+w/8, y+7*h/8);
    fl_vertex(x+7*w/8, y+7*h/8);
    fl_vertex(x+7*w/8, y+6*h/8);
    fl_vertex(x+7*w/8-2*w/14, y+6*h/8);
    fl_vertex(x+7*w/8-2*w/14, y+2*h/8);
    fl_vertex(x+7*w/8, y+2*h/8);
    for (int i = 0; i < 8; i++){
        fl_vertex(x+7*w/8-3*i*w/28, y+2*h/8-h/8-(3*h/28)*((i+1)%2));
        if (i == 7){
            break;
        }
        fl_vertex(x+7*w/8-3*(i+1)*w/28, y+2*h/8-h/8-(3*h/28)*((i+1)%2));
    }
    fl_vertex(x+w/8, y+2*h/8);
    fl_vertex(x+w/8+2*w/14, y+2*h/8);
    fl_vertex(x+w/8+2*w/14, y+6*h/8);
    fl_vertex(x+w/8, y+6*h/8);
    fl_vertex(x+w/8, y+7*h/8);
    fl_end_complex_polygon();
    fl_color(c1);
    fl_line_style(0, 2);
    fl_line(x+w/8+3*w/14, y+2*h/8, x+7*w/8-3*w/14, y+2*h/8);
    fl_line(x+w/8+3*w/14, y+6*h/8, x+7*w/8-3*w/14, y+6*h/8);
    if (white){
        
        fl_begin_loop();
        fl_vertex(x+w/8, y+7*h/8);
        fl_vertex(x+7*w/8, y+7*h/8);
        fl_vertex(x+7*w/8, y+6*h/8);
        fl_vertex(x+7*w/8-2*w/14, y+6*h/8);
        fl_vertex(x+7*w/8-2*w/14, y+2*h/8);
        fl_vertex(x+7*w/8, y+2*h/8);
        for (int i = 0; i < 8; i++){
            fl_vertex(x+7*w/8-3*i*w/28, y+2*h/8-h/8-(3*h/28)*((i+1)%2));
            if (i == 7){
                break;
            }
            fl_vertex(x+7*w/8-3*(i+1)*w/28, y+2*h/8-h/8-(3*h/28)*((i+1)%2));
        }
        fl_vertex(x+w/8, y+2*h/8);
        fl_vertex(x+w/8+2*w/14, y+2*h/8);
        fl_vertex(x+w/8+2*w/14, y+6*h/8);
        fl_vertex(x+w/8, y+6*h/8);
        fl_vertex(x+w/8, y+7*h/8);
        fl_end_loop();
    }


}
void CU::queen(int i, bool white){
    int x = get_xm(i), y = get_ym(i), w = get_wm(i), h = get_hm(i);
    int c = piece_white, c1 = piece_black;
    if (!white){
        c = piece_black;
        c1 = piece_white;
    }
    fl_color(c);
    fl_begin_complex_polygon();
    fl_vertex(x+3*w/16, y+7*h/8);
    fl_vertex(x+13*w/16, y+7*h/8);
    fl_vertex(x+13*w/16, y+11*h/16);
    fl_vertex(x+12*w/16, y+11*h/16);
    fl_vertex(x+13*w/16, y+2*h/8);
    fl_vertex(x+10*w/16, y+11*h/16);
    fl_vertex(x+9*w/16+2, y+3*h/16);
    fl_vertex(x+8*w/16, y+11*h/16);
    fl_vertex(x+7*w/16-2, y+3*h/16);
    fl_vertex(x+6*w/16, y+11*h/16);
    fl_vertex(x+3*w/16, y+2*h/8);
    fl_vertex(x+4*w/16, y+11*h/16);
    fl_vertex(x+3*w/16, y+11*h/16);
    fl_end_complex_polygon();
    fl_pie(x+9*w/16-w/16+2, y+3*h/16-h/16, w/8, h/8, 0, 360);
    fl_pie(x+7*w/16-w/16-2, y+3*h/16-h/16, w/8, h/8, 0, 360);
    fl_pie(x+13*w/16-w/16, y+4*h/16-h/16, w/8, h/8, 0, 360);
    fl_pie(x+3*w/16-w/16, y+4*h/16-h/16, w/8, h/8, 0, 360);
    fl_color(c1);
    fl_line_style(0, 2);
    fl_line(x+w/4, y+6*h/8, x+3*w/4, y+6*h/8);
    if (white){
        fl_begin_loop();
        fl_vertex(x+3*w/16, y+7*h/8);
        fl_vertex(x+13*w/16, y+7*h/8);
        fl_vertex(x+13*w/16, y+11*h/16);
        fl_vertex(x+12*w/16, y+11*h/16);
        fl_vertex(x+13*w/16, y+2*h/8);
        fl_vertex(x+10*w/16, y+11*h/16);
        fl_vertex(x+9*w/16+2, y+3*h/16);
        fl_vertex(x+8*w/16, y+11*h/16);
        fl_vertex(x+7*w/16-2, y+3*h/16);
        fl_vertex(x+6*w/16, y+11*h/16);
        fl_vertex(x+3*w/16, y+2*h/8);
        fl_vertex(x+4*w/16, y+11*h/16);
        fl_vertex(x+3*w/16, y+11*h/16);
        fl_end_loop();
    }
    fl_color(c);
    fl_pie(x+9*w/16-w/16+2, y+3*h/16-h/16, w/8, h/8, 0, 360);
    fl_pie(x+7*w/16-w/16-2, y+3*h/16-h/16, w/8, h/8, 0, 360);
    fl_pie(x+13*w/16-w/16, y+4*h/16-h/16, w/8, h/8, 0, 360);
    fl_pie(x+3*w/16-w/16, y+4*h/16-h/16, w/8, h/8, 0, 360);
    if (white){
        fl_color(c1);
        fl_arc(x+9*w/16-w/16+2, y+3*h/16-h/16, w/8, h/8, 0, 360);
        fl_arc(x+7*w/16-w/16-2, y+3*h/16-h/16, w/8, h/8, 0, 360);
        fl_arc(x+13*w/16-w/16, y+4*h/16-h/16, w/8, h/8, 0, 360);
        fl_arc(x+3*w/16-w/16, y+4*h/16-h/16, w/8, h/8, 0, 360);
    }
}
void CU::king(int i, bool white){
    int x = get_xm(i), y = get_ym(i), w = get_wm(i), h = get_hm(i);
    int c = piece_white, c1 = piece_black;
    if (!white){
        c = piece_black;
        c1 = piece_white;
    }
    fl_color(c);
    fl_pie(x+w/2, y+h/4, 3*w/8, 5*h/8, -60, 240);
    fl_pie(x+w/8, y+h/4, 3*w/8, 5*h/8, -60, 240);
    fl_color(c1);
    if (white){
        fl_line_style(0, 2);
        fl_arc(x+w/2, y+h/4, 3*w/8, 5*h/8, -60, 240);
        fl_arc(x+w/8, y+h/4, 3*w/8, 5*h/8, -60, 240);
    }
    fl_color(c);
    fl_polygon(x+3*w/16, y+7*h/8, x+13*w/16, y+7*h/8, x+13*w/16, y+11*h/16, x+3*w/16, y+11*h/16);
    fl_line_style(0, 2);
    fl_color(c1);
    if (white){
        fl_line(x+3*w/16, y+7*h/8, x+13*w/16, y+7*h/8);
        fl_line(x+13*w/16, y+7*h/8, x+13*w/16, y+11*h/16+h/32);
        fl_line(x+3*w/16, y+7*h/8, x+3*w/16, y+11*h/16+h/32);
    }
    fl_line(x+w/4, y+6*h/8-2, x+3*w/4, y+6*h/8-2);
    fl_line_style(0, 4);
    fl_color(piece_black);
    fl_line(x+w/2, y+11*h/16, x+w/2, y+h/16);
    fl_line(x+13*w/32, y+3*h/16, x+19*w/32, y+3*h/16);
}