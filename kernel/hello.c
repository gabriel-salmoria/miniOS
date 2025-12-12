void main() {
    // Pointer to video memory
    char* video_memory = (char*) 0xb8000;

    // Write 'K' to the top-left corner
    *video_memory = 'K'; 
    *(video_memory + 1) = 0x0f; // White text on black background
    
    // Write 'C' next to it
    *(video_memory + 2) = 'C';
    *(video_memory + 3) = 0x0f;
}
