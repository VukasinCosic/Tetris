#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)


int n_field_width = 12;
int n_field_height = 18;

int n_current_piece = rand() % 7;
int n_current_rotation = 0;
int n_current_x = n_field_width / 2;
int n_current_y = 0;
int n_speed = 20;
int n_speed_counter = 0;
int n_piece_count = 0;
int score = 0;
bool block_force_down = false;
std::vector<int> full_lines;

int Rotate(int px, int py, int r)
{
	int pi = 0;
	switch (r % 4)
	{
	case 0: // 0 degrees			// 0  1  2  3
		pi = py * 4 + px;			// 4  5  6  7
		break;						// 8  9 10 11
									//12 13 14 15

	case 1: // 90 degrees			//12  8  4  0
		pi = 12 + py - (px * 4);	//13  9  5  1
		break;						//14 10  6  2
									//15 11  7  3

	case 2: // 180 degrees			//15 14 13 12
		pi = 15 - (py * 4) - px;	//11 10  9  8
		break;						// 7  6  5  4
									// 3  2  1  0

	case 3: // 270 degrees			// 3  7 11 15
		pi = 3 - py + (px * 4);		// 2  6 10 14
		break;						// 1  5  9 13
	}								// 0  4  8 12

	return pi;
}


bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY, unsigned char* playing_field)
{
	// All Field cells >0 are occupied
	for (int px = 0; px < 4; px++)
		for (int py = 0; py < 4; py++)
		{
			// Get index into piece
			int pi = Rotate(px, py, nRotation);

			// Get index into field
			int fi = (nPosY + py) * n_field_width + (nPosX + px);

			// Check that test is in bounds
			if (nPosX + px >= 0 && nPosX + px < n_field_width)
			{
				if (nPosY + py >= 0 && nPosY + py < n_field_height)
				{
					// In Bounds so do collision check
					if (tetromino[nTetromino][pi] != L'.' && playing_field[fi] != 0)
						return false; // fail on first hit
				}
			}
		}

	return true;
}


internal int simulate_game(Input* input, unsigned char* playing_field)
{
	clear_screen(0x000000);
	//draw_borders();
	draw_number(score, 5.f, 45.f, 1.f, 0xbbffbb);

	n_speed_counter++;
	if (n_speed_counter == n_speed)
		block_force_down = true;

	if ((is_down(BUTTON_LEFT)) && DoesPieceFit(n_current_piece, n_current_rotation, n_current_x - 1, n_current_y, playing_field))
	{
		n_current_x--;
	}

	if ((is_down(BUTTON_RIGHT)) && DoesPieceFit(n_current_piece, n_current_rotation, n_current_x + 1, n_current_y, playing_field))
	{
		n_current_x++;
	}

	if ((is_down(BUTTON_DOWN)) && DoesPieceFit(n_current_piece, n_current_rotation, n_current_x, n_current_y + 1, playing_field))
	{
		n_current_y++;
	}

	if ((pressed(BUTTON_UP)) && DoesPieceFit(n_current_piece, n_current_rotation + 1, n_current_x, n_current_y, playing_field))
	{
		n_current_rotation++;
	}

	if (block_force_down)
	{
		if (DoesPieceFit(n_current_piece, n_current_rotation, n_current_x, n_current_y + 1, playing_field))
		{
			n_current_y++;
		}
		else
		{
			//Lock piece
			for (int px = 0; px < 4; px++)
				for (int py = 0; py < 4; py++)
					if (tetromino[n_current_piece][Rotate(px, py, n_current_rotation)] != L'.')
						playing_field[(n_current_y + py) * n_field_width + (n_current_x + px)] = n_current_piece + 1;

			//Check if it made a line
			for (int py = 0; py < 4; py++)
				if (n_current_y + py < n_field_height - 1)
				{
					bool bLine = true;
					for (int px = 1; px < n_field_width - 1; px++)
						bLine &= (playing_field[(n_current_y + py) * n_field_width + px]) != 0;

					if (bLine)
					{
						// Remove line
						for (int px = 1; px < n_field_width - 1; px++)
							playing_field[(n_current_y + py) * n_field_width + px] = 8;
						full_lines.push_back(n_current_y + py);
					}
				}

			//New piece
			n_current_piece = rand() % 7;
			n_current_rotation = 0;
			n_current_x = n_field_width / 2;
			n_current_y = 0;
			n_piece_count++;

			score += 25;
			if (!full_lines.empty())
				score += full_lines.size() * 150;

			running = DoesPieceFit(n_current_piece, n_current_rotation, n_current_x, n_current_y, playing_field);
		}
		n_speed_counter = 0;
		block_force_down = false;

		if (n_piece_count % 25 == 0)
			if (n_speed >= 10) n_speed--;
		
	}

	for (int x = 0; x < n_field_width; x++) // Draw board
		for (int y = 0; y < n_field_height; y++)
		{
			switch (playing_field[y * n_field_width + x])
			{
			case(1):
			{
				draw_rect((x - 5) * 4, -((y - 8) * 4), 2.f, 2.f, 0x00fff0);
			}break;
			case(2):
			{
				draw_rect((x - 5) * 4, -((y - 8) * 4), 2.f, 2.f, 0xfd7aca);
			}break;
			case(3):
			{
				draw_rect((x - 5) * 4, -((y - 8) * 4), 2.f, 2.f, 0xffff00);
			}break;
			case(4):
			{
				draw_rect((x - 5) * 4, -((y - 8) * 4), 2.f, 2.f, 0x00ff00);
			}break;
			case(5):
			{
				draw_rect((x - 5) * 4, -((y - 8) * 4), 2.f, 2.f, 0xff0000);
			}break;
			case(6):
			{
				draw_rect((x - 5) * 4, -((y - 8) * 4), 2.f, 2.f, 0xff8000);
			}break;
			case(7):
			{
				draw_rect((x - 5) * 4, -((y - 8) * 4), 2.f, 2.f, 0xff00f0);
			}break;
			case(8):
			{
				draw_rect((x - 5) * 4, -((y - 8) * 4), 2.f, 2.f, 0x9c9c9c);
			}break;
			case(9):
			{
				draw_rect((x - 5) * 4, -((y - 8) * 4), 2.f, 2.f, 0xc9c9c9);
			}break;
			default:
			{
			}
			}
		}

	for (int px = 0; px < 4; px++) // Draw current tetromino
		for (int py = 0; py < 4; py++)
			if (tetromino[n_current_piece][Rotate(px, py, n_current_rotation)] != L'.')
			{
				switch (n_current_piece)
				{
				case(0):
				{
					draw_rect((n_current_x - 5 + px) * 4, -((n_current_y + py - 8) * 4), 2.f, 2.f, 0x00fff0);
				}break;
				case(1):
				{
					draw_rect((n_current_x - 5 + px) * 4, -((n_current_y + py - 8) * 4), 2.f, 2.f, 0xfd7aca);
				}break;
				case(2):
				{
					draw_rect((n_current_x - 5 + px) * 4, -((n_current_y + py - 8) * 4), 2.f, 2.f, 0xffff00);
				}break;
				case(3):
				{
					draw_rect((n_current_x - 5 + px) * 4, -((n_current_y + py - 8) * 4), 2.f, 2.f, 0x00ff00);
				}break;
				case(4):
				{
					draw_rect((n_current_x - 5 + px) * 4, -((n_current_y + py - 8) * 4), 2.f, 2.f, 0xff0000);
				}break;
				case(5):
				{
					draw_rect((n_current_x - 5 + px) * 4, -((n_current_y + py - 8) * 4), 2.f, 2.f, 0xff8000);
				}break;
				case(6):
				{
					draw_rect((n_current_x - 5 + px) * 4, -((n_current_y + py - 8) * 4), 2.f, 2.f, 0xff00f0);
				}break;
				}
			}

	if (!full_lines.empty())
	{
		// Delete lines

		for (auto& v : full_lines)
			for (int px = 1; px < n_field_width - 1; px++)
			{
				for (int py = v; py > 0; py--)
					playing_field[py * n_field_width + px] = playing_field[(py - 1) * n_field_width + px];
				playing_field[px] = 0;
			}

		full_lines.clear();
	}

	return score;
}
