

#include "cub3d.h"

// Function to draw a square at (x, y) with a specific color
void draw_square(t_game *game, int x, int y, int color) {
    int start_x = x * TILE_SIZE;
    int start_y = y * TILE_SIZE;

    for (int i = 0; i < TILE_SIZE; i++) {
        for (int j = 0; j < TILE_SIZE; j++) {
            mlx_pixel_put(game->mlx, game->win, start_x + i, start_y + j, color);
        }
    }
}

void draw_player(t_game *game, int color) {
    int start_x = game->player_x * (TILE_SIZE / 2);
    int start_y = game->player_y * (TILE_SIZE / 2);

    color = COLOR_START;
    printf("start_x and start_y: %d, %d\n", start_x, start_y);
    for (int i = 0; i < TILE_SIZE; i++) {
        for (int j = 0; j < TILE_SIZE; j++) {
            mlx_pixel_put(game->mlx, game->win, start_x + i, start_y + j, color);
        }
    }
}

// Function to render the map and the player
void render_map(t_game *game) {
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            if (game->map[y][x] == '1') {
                draw_square(game, x, y, COLOR_WALL);
            } else if (game->map[y][x] == '0') {
                draw_square(game, x, y, COLOR_PATH);
            } else if (game->map[y][x] == 'S') {    
                // draw_square(game, x, y, COLOR_START);
                if (game->changed == 0) {
                    game->player_x = x;
                    game->player_y = y;
                    game->changed = 1;
                }
            }
        }
    }
    // Draw the player
    draw_player(game, COLOR_PLAYER);
}

// Function to handle player movement
int handle_keypress(int keycode, t_game *game) {
    int new_x = game->player_x;
    int new_y = game->player_y;
    int pressed = 0;

    printf("Key pressed: %d\n", keycode);
    if (keycode == KEY_W) 
    {
        new_y--; // Move up
        pressed = 1;
    }
    if (keycode == KEY_A)
    {
        new_x--; // Move left
        pressed = 1;
    }
    if (keycode == KEY_S)
    {
        new_y++; // Move down
        pressed = 1;
    }
    if (keycode == KEY_D)
    {
        new_x++; // Move right
        pressed = 1;
    }

    // Check for valid movement (not walking into walls)
    if (pressed && new_x >= 0 && new_x < 720 && new_y >= 0 && new_y < 720) {
        game->player_x = new_x;
        game->player_y = new_y;
        printf("Player moved to (%d, %d)\n", game->player_x, game->player_y);

        // Redraw the map and player
        mlx_clear_window(game->mlx, game->win);
        // render_map(game);
        raycast(game);
        pressed = 0;
    }
    // Exit if ESC is pressed
    if (keycode == KEY_ESC) {
        mlx_destroy_window(game->mlx, game->win);
        exit(0);
    }
    return 0;
}

void draw_wall(int x, int wall_height, int color, t_game *game)
{
    int start = WIN_HEIGHT / 2 - wall_height / 2;
    int end = WIN_HEIGHT / 2 + wall_height / 2;

    for (int y = start; y < end; y++)
    {
        if (y >= 0 && y < WIN_HEIGHT)
        {
            printf("drawing wall (%d, %d)\n", x, y);
            mlx_pixel_put(game->mlx, game->win, x, y, color);
        }
    }
}

void raycast(t_game *game)
{
    int FOV = PI / 3;  // 60 degrees
    int ray = 1;
    int MAX_DEPTH = 800;
    int depth = 0;
    int ray_x, ray_y;
    int ray_angle;
    int map_x, map_y;
    int distance;
    int wall_height;
    int color, color_intensity;


    while (ray < game->NUM_RAY)
    {
        ray_angle = game->player_angle - FOV / 2 + (ray / game->NUM_RAY) * FOV;
        depth = 1;
        while (depth < MAX_DEPTH)
        {
            ray_x = game->player_x  + depth * cos(ray_angle);
            ray_y = game->player_y + depth * sin(ray_angle);

            map_x = (int)(ray_x / TILE_SIZE);
            map_y =  (int)(ray_y / TILE_SIZE);
            //wall collision check 
            if (game->map[map_x][map_y])
            {
                // calc dist & wall height 
                distance = depth * cos(ray_angle - game->player_angle);
                wall_height = WIN_HEIGHT / (distance + 0.0001);
                color_intensity = 255 - fmin(distance, 255);
                color = (color_intensity << 16);
                draw_wall(ray * (WIN_WIDTH / game->NUM_RAY), wall_height, color, game);
                break;
            }
            depth++;
        }
        ray++;
    }
}

int main() {
    // Initialize game structure
    t_game game = {
        .map = {
            "1111111111111111111",
            "1001001001001001001",
            "1001001001001001001",
            "1001001001001001001",
            "1001001000001001001",
            "100000000S000000001",
            "1000011111111000001",
            "1111000000000001111",
            "1000000101010000001",
            "1111111111111111111"
        }
    };
    game.changed = 0;
    // Initialize MiniLibX
    game.mlx = mlx_init();
    game.win = mlx_new_window(game.mlx, WIN_WIDTH, WIN_HEIGHT, "Map with Moving Player");

    // Initial rendering of the map and player
    // render_map(&game);
    raycast(&game);

    // Set key press hook
    mlx_key_hook(game.win, handle_keypress, &game);
    
    // Run the event loop
    mlx_loop(game.mlx);

    return 0;
}
