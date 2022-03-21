-- constants
w_pressed = 26
a_pressed = 4
s_pressed = 22
d_pressed = 7

space_pressed = 44

left_mouse_click = 0

playerHp = 100
is_dashing = false
dash_timer = 0
dashTime = 0.2
dash_speed_multiplyer = 4
player_collided_with_enemy = false
damage_cooldown_timer = 2

playerSpeed = 350

-- returns player Hp
function returnPlayerHp()
	return playerHp
end

-- returns players dash_speed_multiplyer
function dashSpeedMultiplyer()
	return dash_speed_multiplyer
end

-- d pressed moving right
if is_pressed(d_pressed) then
	move(playerSpeed * deltaTime(), 0)
end

-- w pressed moving up
if is_pressed(w_pressed) then
	move(0, playerSpeed * deltaTime())
end

-- a pressed moving left
if is_pressed(a_pressed) then
	move(-playerSpeed * deltaTime(), 0)
end

-- s pressed moving down
if is_pressed(s_pressed) then
	move(0, -playerSpeed * deltaTime())
end

-- space pressed then dash
if is_triggered(space_pressed) then
	dash()
end

-- mouse left button clicked then shoot bullet
if is_mouse_button_triggered(left_mouse_click) then
	x = (mouse_pos_x() - 400) + xPos_of_cam()
	y = -1 * (mouse_pos_y() - 400) + yPos_of_cam()
	print("called")
	fire(0, x, y)
end

-- called when player collides with enemy
function PlayerCollidedWithEnemy()
	player_is_dead()
end