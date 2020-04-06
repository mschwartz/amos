/** @file
 * \brief Resource compiler for game engine.

 *
 * Reads in PNG files and generates a packed resource file along with a
 * Resources.h file. The Resources.h file contains #defines for each resource,
 * which are ultimately indexes into an array of BBitmaps in the game engine.
 *
 * TODO:
 *  Support song data, raw 8 bit samples, etc.
 */

#include "rcomp.h"

//extern ResourceFile resourceFile;

void usage() {
  printf("Usage: rcomp <option(s)> <files>\n");
  printf(" Compiles resources specified in <files> into one packed binary "
         "file, Resources.bin.\n\n");
  printf(" The <files> are resource 'source' files, which contain one "
         "'resource' filename per line.\n\n");
  printf(" The compiler also generates a .h file, Resources.h, with\n");
  printf(" a #define for each of the input resources, an index into the "
         "offset\n");
  printf(" table generated at the head of the packed binary file.\n\n");
  printf(" Bitmaps are converted from BMP format into a simpler raw format\n");
  printf(" that allows the game/application to process the graphics with\n");
  printf(" less overhead.\n");

  exit(1);
}


void process_path(char *line) {
  char work[MAX_STRING_LENGTH], base[MAX_STRING_LENGTH];

  parse_token(work, line);
  // dirname and basename may modify their buffers
  strcpy(base, work);
  sprintf(resourceFile.path, "%s/%s", dirname(work), basename(base));
  // assure path ends with /
  printf("\n\nProcessing resources in path (%s)\n", resourceFile.path);
}

#ifndef ENABLE_RAW
void process_raw(char *line) {
  char work[MAX_STRING_LENGTH], base[MAX_STRING_LENGTH];

  // this code is common with BITMAP logic
  parse_token(base, line);
  sprintf(work, "%s/%s", resourceFile.path, base);

  resourceFile.StartResource(base);

  // OUTPUT format is TUInt32 size, TUint8 data[size]
  auto *r = new RawFile(work);
  if (!r->alive) {
    Panic("*** RawFile Can't open %s\n", work);
  }
  printf("%s: %d bytes\n", r->filename, r->size);

  resourceFile.Write(&r->size, sizeof(r->size));
  resourceFile.Write(r->data, r->size);
}
#endif

TUint16 process_bitmap(char *line) {
  char work[MAX_STRING_LENGTH], base[MAX_STRING_LENGTH];

  parse_token(base, line);
  sprintf(work, "%s/%s", resourceFile.path, base);
  TUint16 id = resourceFile.StartResource(base);

  BMPFile b(work);
  b.Dump();

  // copy the bitmap values into output
  b.Write(resourceFile);
  return id;
}

#ifdef ENABLE_TILEMAPS
void process_tilemap(char *line) {
  char work[MAX_STRING_LENGTH], filename[MAX_STRING_LENGTH];

  parse_token(filename, line);
  sprintf(work, "%s/%s", resourceFile.path, filename);
  printf("TILEMAP %s\n", filename);

  TileMap map(resourceFile.path, filename);
  map.Write(resourceFile);
}

#ifdef ENABLE_TMX
void process_tmx(char *line) {
  char work[MAX_STRING_LENGTH], filename[MAX_STRING_LENGTH];

  parse_token(filename, line);
  sprintf(work, "%s/%s", resourceFile.path, filename);
  printf("TILEMAP %s\n", filename);

  TMXFile file(resourceFile.path, filename);
//  map.Write(resourceFile);
}
#endif
#endif

void process_palette(char *line) {
  char work[MAX_STRING_LENGTH], base[MAX_STRING_LENGTH];

  parse_token(base, line);
  sprintf(work, "%s/%s", resourceFile.path, base);
  sprintf(resourceFile.path, "%s/%s", dirname(work), basename(base));
  auto *r = new RawFile(work);
  if (!r->alive) {
    Panic("*** process_palette can't open %s\n", work);
  }
  printf("%s: %d bytes\n", r->filename, r->size);
  resourceFile.StartResource(base);
  resourceFile.Write(r->data, 256 * 3);
}

#ifdef ENABLE_SPRITESHEETS
void process_spritesheet(char *line) {
  char *ptr = line;
  char dimensions[256];
  char filename[256];
  ptr = parse_token(dimensions, ptr);
  ptr = parse_token(filename, ptr);
  printf("SPRITESHEET (%s) dimensions: %s\n", filename, dimensions);

  char work[MAX_STRING_LENGTH];
  sprintf(work, "%s/%s", resourceFile.path, filename);
  SpriteSheet spriteSheet(dimensions, work);
//  printf("WRITE\n");
  spriteSheet.Write(resourceFile);
}
#endif

void handle_file(char *fn) {
  char line[MAX_STRING_LENGTH], token[MAX_STRING_LENGTH];

  RawFile file(fn);
  if (!file.alive) {
    Panic("*** handle_file: Can't open input file %s (%d)\n", fn, errno);
  }

  while (file.ReadLine(line)) {
    const TInt num_read = strlen(line);
    for (TInt i = 0; i < num_read; i++) {
      if (i && line[i - 1] != ' ' && line[i] == '#') {
        continue;
      }
      if (line[i] == '#' || line[i] == '\n') {
        line[i] = '\0';
        break;
      }
    }

    char *ptr = parse_token(token, line);
    if (!strlen(token)) {
      continue;
    } 
    else if (!strcasecmp(token, "PATH")) {
      process_path(ptr);
    } 
#ifdef ENABLE_RAW
    else if (!strcasecmp(token, "RAW")) {
      process_raw(ptr);
    } 
#endif
    else if (!strcasecmp(token, "BITMAP")) {
      process_bitmap(ptr);
    } 
#ifdef ENABLE_TILEMAPS
    else if (!strcasecmp(token, "TILEMAP")) {
      process_tilemap(ptr);
    } 
    else if (!strcasecmp(token, "TMX")) {
      process_tmx(ptr);
    } 
#endif
    else if (!strcasecmp(token, "PALETTE")) {
      process_palette(ptr);
    } 
#ifdef ENABLE_SPRITESHEETS
    else if (!strcasecmp(token, "SPRITESHEET")) {
      process_spritesheet(ptr);
    } 
#endif
    else {
      printf("INVALID directive %s\n", line);
    }
  }
}

TInt main(TInt ac, char *av[]) {
  if (ac < 2) {
    usage();
  }

  // process all files on command line
  for (TInt i = 1; i < ac; i++) {
    handle_file(av[i]);
  }

  resourceFile.Finish();

  return 0;
}
