Module.onRuntimeInitialized = function() {
    Module.canvas = document.getElementById('canvas');
    initEmulator();
};

async function loadRomFromFile() {
    const fileInput = document.getElementById('romFile');
    const file = fileInput.files[0];
    if (!file) {
        alert("Please select a ROM file!");
        return;
    }

    const buffer = await file.arrayBuffer();
    const romData = new Uint8Array(buffer);

    try {
        FS.writeFile("/rom.nes", romData);

        if (Module.load_rom_from_file("/rom.nes"))
            console.log("ROM loaded");
        else
            console.error("Failed to load ROM");
    } catch (err) {
        console.error("Failed to load ROM (", err, ")");
    }
}

function initEmulator() {
    try {
        Module.init_emulator();
    } catch (err) {
        console.error("An error occured while initializing the emulator (", err, ")");
    }
}

function startEmulator() {
    try {
        Module.start_emulator();
    } catch (err) {
        console.error("An error occured while starting the emulator (", err, ")");
    }
}

function stopEmulator() {
    try {
        Module.stop_emulator();
    } catch (err) {
        console.error("An error occured while stopping the emulator (", err, ")");
    }
}
