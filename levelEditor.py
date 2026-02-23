import pygame, time, math, os, json, numpy
from tkinter import filedialog
# from pathlib import Path

WIDTH = 1400
HEIGHT = 800
SCALE = 2

TILE_SIZE = 12
TOTAL_TILE_SIZE = SCALE * TILE_SIZE
CHUNK_SIZE = 8

LINE_WIDTH = 2

tiles = []

pygame.init()

file = filedialog.askdirectory()

levelFile = file + "/level.bin"
print(levelFile)
configFile = file + "/levelConfig.json"
print(configFile)

class Editor:
    def __init__(self):

        print(pygame.display.Info())

        self.screen = pygame.Surface((WIDTH, HEIGHT))
        self.display = pygame.display.set_mode((WIDTH, HEIGHT), flags=pygame.RESIZABLE)
        self.clock = pygame.time.Clock()
        
        self.dt = 1
        self.last_time = time.time() - 1/60

        self.running = True
        self.controls = {'left': False, 'right': False, 'up': False, 'down': False,
                         'mouse_1': False, 'mouse_3': False}

        self.scroll_speed = 5 + SCALE * 2
        self.scroll = pygame.Vector2(0, 0)

        self.mouse = pygame.Vector2(0, 0)

        self.block = pygame.image.load("data/images/blocks.png").convert_alpha()
        self.data = self.load("data/levels/blockData.json")["tiles"]
        self.temp_tile = self.data[0]["pos"]
        self.tileID = 0

        pygame.font.init()

        self.byteBounce = pygame.font.Font("data/fonts/ByteBounce.ttf", 30)
        self.text_surface = self.byteBounce.render(self.data[self.tileID]['type'], False, (255, 255, 255))

    def load(self, path):
        jsonFile = numpy.fromfile(levelFile, [("x", numpy.int16), ("y", numpy.int16), ("type", numpy.int16)])
        for i in range(len(jsonFile)):
            tiles.append([int(jsonFile[i][0]), int(jsonFile[i][1]), int(jsonFile[i][2])])
        with open(path, 'r') as f:
            data = json.load(f)
            return(data)

        
    def save(self, data, path):
        # temp = data.ravel()
        new = numpy.array(sum(data, []))
        numpy.astype(new, numpy.int16).tofile(path)
    
    def close(self):
        pygame.quit()
        os._exit(0)

    def update(self):

        self.mouse.x = pygame.mouse.get_pos()[0]
        self.mouse.y = pygame.mouse.get_pos()[1]

        self.scroll.x += (self.controls['right'] - self.controls['left']) * self.scroll_speed * self.dt
        self.scroll.y += (self.controls['down'] - self.controls['up']) * self.scroll_speed * self.dt

        self.render_scroll = [int(self.scroll.x - self.screen.get_width() * 0.5), int(self.scroll.y - self.screen.get_height() * 0.5)]

        self.screen.fill((10, 10, 10))
        self.draw_tile_grid([TOTAL_TILE_SIZE + LINE_WIDTH, TOTAL_TILE_SIZE + LINE_WIDTH], (20, 20, 20))
        self.draw_tile_grid([CHUNK_SIZE * (TOTAL_TILE_SIZE + LINE_WIDTH), CHUNK_SIZE * (TOTAL_TILE_SIZE + LINE_WIDTH)], (50, 25, 35))
        self.draw_tile_grid([10000000, 10000000], (35, 45, 70))
        for i in range(len(tiles)):
            self.draw([tiles[i][0], tiles[i][1]], self.clip(self.block, 
                                (self.data[tiles[i][2]]["pos"][0], self.data[tiles[i][2]]["pos"][1], TILE_SIZE, TILE_SIZE)))

        self.draw_cursor()
        self.screen.blit(self.text_surface, (10, 0))
        
    def draw_cursor(self):
        if self.controls['mouse_1'] == True or self.controls['mouse_3'] == True:
            self.save(tiles, levelFile)

        self.temp_tile_pos = [-(self.mouse.x + self.render_scroll[0]) % (TOTAL_TILE_SIZE + LINE_WIDTH) - TOTAL_TILE_SIZE + self.mouse.x, 
                              -(self.mouse.y + self.render_scroll[1]) % (TOTAL_TILE_SIZE + LINE_WIDTH) - TOTAL_TILE_SIZE + self.mouse.y]
        if self.controls['mouse_3'] != True:
            self.rect = pygame.Rect(self.temp_tile_pos[0], self.temp_tile_pos[1], TOTAL_TILE_SIZE, TOTAL_TILE_SIZE)
            self.screen.blit(pygame.transform.scale(self.clip(self.block, (self.temp_tile[0], self.temp_tile[1], TILE_SIZE, TILE_SIZE)), 
                                                    (TOTAL_TILE_SIZE, TOTAL_TILE_SIZE)), self.rect)
        
        if self.controls['mouse_1'] != True:
            self.surface = pygame.Surface((TOTAL_TILE_SIZE, TOTAL_TILE_SIZE))
            self.surface.set_alpha(40)
            self.surface.fill((255, 255, 255))
            self.screen.blit(self.surface, self.temp_tile_pos)
        else:
            self.temp_tile_pos = [math.floor((self.mouse.x + self.render_scroll[0] - LINE_WIDTH * 0.5) / (TOTAL_TILE_SIZE + LINE_WIDTH)), 
                                 -math.floor((self.mouse.y + self.render_scroll[1] - LINE_WIDTH * 0.5) / (TOTAL_TILE_SIZE + LINE_WIDTH)) - 1]
            for i in range(len(tiles)):
                if tiles[i][0] == self.temp_tile_pos[0] and tiles[i][1] == self.temp_tile_pos[1]:
                    tiles[i] = [self.temp_tile_pos[0], self.temp_tile_pos[1], self.tileID]
                    return
            tiles.append([self.temp_tile_pos[0], self.temp_tile_pos[1], self.tileID])
        self.temp_tile_pos = [math.floor((self.mouse.x + self.render_scroll[0] - LINE_WIDTH * 0.5) / (TOTAL_TILE_SIZE + LINE_WIDTH)), 
                             -math.floor((self.mouse.y + self.render_scroll[1] - LINE_WIDTH * 0.5) / (TOTAL_TILE_SIZE + LINE_WIDTH)) - 1]
        if self.controls['mouse_3'] == True:
            for i in range(len(tiles)):
                if tiles[i][0] == self.temp_tile_pos[0] and tiles[i][1] == self.temp_tile_pos[1]:
                    tiles.pop(i)
                    return


    def draw_tile_grid(self, tile_size: list | tuple, color):
        length = math.ceil(self.screen.get_width() / tile_size[0]) + 2
        height = math.ceil(self.screen.get_height() / tile_size[1]) + 2
        for x in range(length):
            pygame.draw.line(self.screen, color, ((x - 1) * tile_size[0] - (self.render_scroll[0] % tile_size[0]), 0), 
                             ((x - 1) * tile_size[0] - (self.render_scroll[0] % tile_size[0]), self.screen.get_height()), width=LINE_WIDTH)
        for y in range(height):
            pygame.draw.line(self.screen, color, (0, (y - 1) * tile_size[1] - (self.render_scroll[1] % tile_size[1])), 
                            (self.screen.get_width(), (y - 1) * tile_size[1] - (self.render_scroll[1] % tile_size[1])), width=LINE_WIDTH)

    def clip(self, surface: pygame.Surface, clip_rect: pygame.Rect) -> pygame.Surface:
        surf = pygame.Surface((clip_rect[2], clip_rect[3]))
        surf.fill((0, 0, 0))
        surf.set_colorkey((0, 0, 0))
        surf.blit(surface, (-clip_rect[0], -clip_rect[1]))
        return(surf)

    def draw(self, pos, image):
        pos[1] += 1
        self.position = [(LINE_WIDTH * (pos[0] + 1) - self.render_scroll[0]) + pos[0] * TOTAL_TILE_SIZE,
                         (LINE_WIDTH * (1 - pos[1]) - self.render_scroll[1]) - pos[1] * TOTAL_TILE_SIZE]
        if (-TOTAL_TILE_SIZE) < self.position[0] < self.screen.get_width():
            if (-TOTAL_TILE_SIZE) < self.position[1] < self.screen.get_height():
                self.screen.blit(pygame.transform.scale(image, (TOTAL_TILE_SIZE, TOTAL_TILE_SIZE)), (self.position[0], self.position[1], TOTAL_TILE_SIZE, TOTAL_TILE_SIZE))

    def run(self):
        while self.running:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    self.running = False
                    self.close()
                if event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_LEFT:
                        self.controls['left'] = True
                    if event.key == pygame.K_RIGHT:
                        self.controls['right'] = True
                    if event.key == pygame.K_UP:
                        self.controls['up'] = True
                    if event.key == pygame.K_DOWN:
                        self.controls['down'] = True
                    if event.key == pygame.K_F11:
                        pygame.display.toggle_fullscreen()
                if event.type == pygame.KEYUP:
                    if event.key == pygame.K_LEFT:
                        self.controls['left'] = False
                    if event.key == pygame.K_RIGHT:
                        self.controls['right'] = False
                    if event.key == pygame.K_UP:
                        self.controls['up'] = False
                    if event.key == pygame.K_DOWN:
                        self.controls['down'] = False
                    if event.key == pygame.K_r:
                        pass
                if event.type == pygame.WINDOWRESIZED:
                    self.screen = pygame.Surface((self.display.get_width(), self.display.get_height()))
                if event.type == pygame.MOUSEBUTTONDOWN:
                    if event.button == 4:
                        try:
                            self.temp_tile = self.data[self.tileID + 1]["pos"]
                            self.tileID += 1
                        except:
                            pass
                    elif event.button == 5:
                        try:
                            if self.tileID > 0:
                                self.temp_tile = self.data[self.tileID - 1]["pos"]
                                self.tileID -= 1
                        except:
                            pass
                    self.text_surface = self.byteBounce.render(self.data[self.tileID]['type'], False, (255, 255, 255))
                    
                    self.controls[f'mouse_{event.button}'] = True
                if event.type == pygame.MOUSEBUTTONUP:
                    self.controls[f'mouse_{event.button}'] = False

            self.dt = time.time() - self.last_time
            self.dt *= 60
            self.last_time = time.time()
            self.update()

            self.display.blit(pygame.transform.scale(self.screen, self.display.get_size()), (0, 0))
            pygame.display.set_caption(f'FPS: {self.clock.get_fps() :.1f}')
            pygame.display.flip()

            self.clock.tick()

Editor().run()