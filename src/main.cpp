// Sound Manager - Interface ImGui

#include "../assets/imgui/imgui.h"
#include "../assets/imgui/backends/imgui_impl_sdl3.h"
#include "../assets/imgui/backends/imgui_impl_sdlrenderer3.h"
#include <SDL3/SDL.h>
#include "Core/Micro/micro.h"
#include "Core/Wav_file/wav_file.h"
#include <thread>
#include <atomic>
#include <vector>
#include <string>

// Variables globales
std::atomic<bool> isRecording(false);
std::atomic<bool> recordingComplete(false);
std::vector<float> currentRecording;
std::string statusMessage = "Prêt";
float recordingProgress = 0.0f;

// Paramètres d'enregistrement
int recordingDuration = 10;
int selectedDevice = 0;

// Paramètres de génération de forme d'onde
int selectedWaveform = 0;
float waveFrequency = 440.0f;
float waveAmplitude = 0.5f;
int waveDuration = 5;
char waveFilename[256] = "generated.wav";
char recordFilename[256] = "enregistrement.wav";

// Statistiques
size_t totalSamples = 0;
float recordedDuration = 0.0f;
float maxAmplitude = 0.0f;

// Fonction d'enregistrement dans un thread
void RecordAudio(int duration) {
    try {
        isRecording = true;
        recordingProgress = 0.0f;
        statusMessage = "Enregistrement en cours...";
        
        SoundManager::Microphone micro;
        
        // Lancer l'enregistrement dans un thread
        auto startTime = std::chrono::steady_clock::now();
        micro.CaptureAudio(duration);
        
        currentRecording = micro.GetRecordedSamples();
        recordingComplete = true;
        isRecording = false;
        recordingProgress = 1.0f;
        
        // Calculer les statistiques
        totalSamples = currentRecording.size();
        recordedDuration = totalSamples / 2.0f / 44100.0f;
        
        maxAmplitude = 0.0f;
        for (const auto& sample : currentRecording) {
            maxAmplitude = std::max(maxAmplitude, std::abs(sample));
        }
        
        statusMessage = "Enregistrement terminé - " + std::to_string(totalSamples) + " échantillons";
        
    } catch (const std::exception& e) {
        statusMessage = " Erreur: " + std::string(e.what());
        isRecording = false;
        recordingProgress = 0.0f;
    }
}

// Fonction pour sauvegarder l'enregistrement
void SaveRecording(const char* filename) {
    if (currentRecording.empty()) {
        statusMessage = " Aucun enregistrement à sauvegarder!";
        return;
    }
    
    try {
        SoundManager::WavFile wavFile;
        wavFile.SetFilename(filename);
        wavFile.SaveWavFile(8, currentRecording);
        statusMessage = "✓ Enregistrement sauvegardé: " + std::string(filename);
    } catch (const std::exception& e) {
        statusMessage = " Erreur lors de la sauvegarde: " + std::string(e.what());
    }
}

// Fonction pour générer une forme d'onde
void GenerateWaveform(int type, float freq, float amp, int dur, const char* filename) {
    if (type == 0) {
        statusMessage = " Sélectionnez un type de forme d'onde!";
        return;
    }
    
    try {
        SoundManager::WavFile wavFile;
        wavFile.SetFrequency(freq);
        wavFile.SetAmplitude(amp);
        wavFile.SetDuration(dur);
        wavFile.SetFilename(filename);
        wavFile.SaveWavFile(type, {});
        statusMessage = "✓ Forme d'onde générée: " + std::string(filename);
    } catch (const std::exception& e) {
        statusMessage = " Erreur: " + std::string(e.what());
    }
}

int main(int argc, char** argv) {
    // Initialisation SDL3
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        return -1;
    }

    // Création de la fenêtre
    SDL_Window* window = SDL_CreateWindow(
        " Sound Manager with Wav_File - ImGui Edition",
        1280, 720,
        SDL_WINDOW_RESIZABLE
    );
    SDL_Surface* icon = SDL_LoadBMP("assets/SMT.bmp");

    if (icon) {
        // 2. Appliquer l'icône à la fenêtre
        SDL_SetWindowIcon(window, icon);
    
        // 3. Libérer la surface (SDL a copié les données dont il avait besoin)
        SDL_DestroySurface(icon);
    } else {
        SDL_Log("Impossible de charger l'icône : %s", SDL_GetError());
    }
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    
    // Configuration ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    // Style
    ImGui::StyleColorsDark();
    
    // Personnalisation des couleurs
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;
    style.FrameRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.ScrollbarRounding = 4.0f;
    
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.30f, 0.55f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.30f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.21f, 0.22f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.20f, 0.25f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.41f, 0.68f, 1.00f);
    
    // Initialisation des backends
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    // Boucle principale
    bool running = true;
    SDL_Event event;
    
    while (running) {
        // Gestion des événements
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                running = false;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
                running = false;
        }

        // Démarrer la frame ImGui
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        // ==================== FENÊTRE PRINCIPALE ====================
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(1280, 720), ImGuiCond_FirstUseEver);
        
        ImGui::Begin("Sound Manager", nullptr, ImGuiWindowFlags_MenuBar);

        // Barre de menu
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Fichier")) {
                if (ImGui::MenuItem("Nouveau")) { /* TODO */ }
                if (ImGui::MenuItem("Ouvrir WAV")) { /* TODO */ }
                ImGui::Separator();
                if (ImGui::MenuItem("Quitter")) { running = false; }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Aide")) {
                if (ImGui::MenuItem("À propos")) { /* TODO */ }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // Onglets
        if (ImGui::BeginTabBar("MainTabs")) {
            
            // ==================== ONGLET ENREGISTREMENT ====================
            if (ImGui::BeginTabItem(" Enregistrement")) {
                ImGui::Spacing();
                
                // Colonne de gauche - Paramètres
                ImGui::BeginChild("RecordParams", ImVec2(400, 0), ImGuiChildFlags_Borders);
                
                ImGui::SeparatorText(" Paramètres d'enregistrement");
                ImGui::Spacing();
                
                // Durée
                ImGui::Text("Durée de l'enregistrement:");
                ImGui::SliderInt("##duration", &recordingDuration, 1, 60, "%d secondes");
                ImGui::Spacing();
                
                // Device
                ImGui::Text("Device audio:");
                const char* devices[] = {"0", "1", "6", "12", "13"};
                ImGui::Combo("##device", &selectedDevice, devices, IM_ARRAYSIZE(devices));
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Changez si le device 0 ne fonctionne pas");
                }
                ImGui::Spacing();
                
                // Nom du fichier
                ImGui::Text("Nom du fichier:");
                ImGui::InputText("##recordfile", recordFilename, IM_ARRAYSIZE(recordFilename));
                ImGui::Spacing();
                
                ImGui::Separator();
                ImGui::Spacing();
                
                // Boutons de contrôle
                ImGui::BeginDisabled(isRecording);
                if (ImGui::Button(" ENREGISTRER", ImVec2(-1, 50))) {
                    std::thread recordThread(RecordAudio, recordingDuration);
                    recordThread.detach();
                }
                ImGui::EndDisabled();
                
                ImGui::Spacing();
                
                ImGui::BeginDisabled(currentRecording.empty() || isRecording);
                if (ImGui::Button(" Sauvegarder", ImVec2(-1, 40))) {
                    SaveRecording(recordFilename);
                }
                ImGui::EndDisabled();
                
                ImGui::Spacing();
                
                if (ImGui::Button(" Effacer", ImVec2(-1, 40))) {
                    currentRecording.clear();
                    totalSamples = 0;
                    recordedDuration = 0.0f;
                    maxAmplitude = 0.0f;
                    statusMessage = "Enregistrement effacé";
                }
                
                ImGui::Spacing();
                if (ImGui::Button(" Lister les devices", ImVec2(-1, 40))) {
                    try {
                        SoundManager::Microphone micro;
                        micro.ListAudioDevicesInformation();
                        statusMessage = "Voir la console pour la liste complète";
                    } catch (...) {
                        statusMessage = "Erreur lors de la récupération des devices";
                    }
                }
                
                ImGui::EndChild();
                
                // Colonne de droite - Visualisation et statistiques
                ImGui::SameLine();
                ImGui::BeginChild("RecordStats", ImVec2(0, 0), ImGuiChildFlags_Borders);
                
                ImGui::SeparatorText(" Statistiques");
                ImGui::Spacing();
                
                // Progression
                if (isRecording) {
                    ImGui::Text(" ENREGISTREMENT EN COURS...");
                    ImGui::ProgressBar(recordingProgress, ImVec2(-1, 0));
                } else if (recordingComplete) {
                    ImGui::TextColored(ImVec4(0, 1, 0, 1), " Enregistrement terminé");
                } else {
                    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1), "En attente...");
                }
                
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                
                // Statistiques détaillées
                ImGui::Text("Échantillons capturés:");
                ImGui::Indent();
                ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1), "%zu échantillons", totalSamples);
                ImGui::Unindent();
                
                ImGui::Spacing();
                ImGui::Text("Durée enregistrée:");
                ImGui::Indent();
                ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1), "%.2f secondes", recordedDuration);
                ImGui::Unindent();
                
                ImGui::Spacing();
                ImGui::Text("Amplitude maximale:");
                ImGui::Indent();
                ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1), "%.4f", maxAmplitude);
                ImGui::Unindent();
                
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                
                // Informations système
                ImGui::SeparatorText(" Configuration");
                ImGui::Text("• Sample Rate: 44100 Hz");
                ImGui::Text("• Canaux: Stéréo (2)");
                ImGui::Text("• Bit Depth: 16-bit PCM");
                ImGui::Text("• Format: WAV");
                
                // Visualisation simple
                if (!currentRecording.empty() && maxAmplitude > 0.0f) {
                    ImGui::Spacing();
                    ImGui::SeparatorText(" Visualisation");
                    
                    float barHeight = maxAmplitude;
                    ImGui::ProgressBar(barHeight, ImVec2(-1, 30), "");
                    ImGui::Text("Niveau audio capturé");
                }
                
                ImGui::EndChild();
                
                ImGui::EndTabItem();
            }
            
            // ==================== ONGLET GÉNÉRATION ====================
            if (ImGui::BeginTabItem(" Générateur de formes d'onde")) {
                ImGui::Spacing();
                
                ImGui::BeginChild("WaveParams", ImVec2(400, 0), ImGuiChildFlags_Borders);
                
                ImGui::SeparatorText(" Paramètres de génération");
                ImGui::Spacing();
                
                // Type de forme d'onde
                ImGui::Text("Type de forme d'onde:");
                const char* waveforms[] = {
                    "Aucune",
                    "Sine (Sinus)",
                    "Cosine (Cosinus)", 
                    "Square (Carrée)",
                    "Triangle (Triangulaire)",
                    "Sawtooth (Dents de scie)",
                    "White Noise (Bruit blanc)",
                    "Pink Noise (Bruit rose)"
                };
                ImGui::Combo("##waveform", &selectedWaveform, waveforms, IM_ARRAYSIZE(waveforms));
                ImGui::Spacing();
                
                // Fréquence
                ImGui::Text("Fréquence (Hz):");
                ImGui::SliderFloat("##frequency", &waveFrequency, 20.0f, 20000.0f, "%.1f Hz", ImGuiSliderFlags_Logarithmic);
                if (ImGui::Button("A4 (440Hz)")) waveFrequency = 440.0f;
                ImGui::SameLine();
                if (ImGui::Button("C4 (261Hz)")) waveFrequency = 261.63f;
                ImGui::SameLine();
                if (ImGui::Button("E4 (329Hz)")) waveFrequency = 329.63f;
                ImGui::Spacing();
                
                // Amplitude
                ImGui::Text("Amplitude:");
                ImGui::SliderFloat("##amplitude", &waveAmplitude, 0.0f, 1.0f, "%.2f");
                ImGui::Spacing();
                
                // Durée
                ImGui::Text("Durée:");
                ImGui::SliderInt("##waveduration", &waveDuration, 1, 30, "%d secondes");
                ImGui::Spacing();
                
                // Nom du fichier
                ImGui::Text("Nom du fichier:");
                ImGui::InputText("##wavefile", waveFilename, IM_ARRAYSIZE(waveFilename));
                ImGui::Spacing();
                
                ImGui::Separator();
                ImGui::Spacing();
                
                // Bouton de génération
                ImGui::BeginDisabled(selectedWaveform == 0);
                if (ImGui::Button(" GÉNÉRER", ImVec2(-1, 50))) {
                    GenerateWaveform(selectedWaveform, waveFrequency, waveAmplitude, waveDuration, waveFilename);
                }
                ImGui::EndDisabled();
                
                ImGui::EndChild();
                
                // Informations
                ImGui::SameLine();
                ImGui::BeginChild("WaveInfo", ImVec2(0, 0), ImGuiChildFlags_Borders);
                
                ImGui::SeparatorText(" Description");
                ImGui::Spacing();
                
                switch (selectedWaveform) {
                    case 1:
                        ImGui::TextWrapped(" Sine Wave (Onde sinusoïdale)");
                        ImGui::Spacing();
                        ImGui::TextWrapped("L'onde la plus pure, contient une seule fréquence. Utilisée pour les tests audio et la synthèse sonore basique.");
                        break;
                    case 2:
                        ImGui::TextWrapped(" Cosine Wave (Onde cosinusoïdale)");
                        ImGui::Spacing();
                        ImGui::TextWrapped("Similaire à l'onde sinusoïdale mais avec un décalage de phase de 90°.");
                        break;
                    case 3:
                        ImGui::TextWrapped(" Square Wave (Onde carrée)");
                        ImGui::Spacing();
                        ImGui::TextWrapped("Riche en harmoniques impaires. Son caractéristique des synthétiseurs vintage et des jeux vidéo 8-bit.");
                        break;
                    case 4:
                        ImGui::TextWrapped(" Triangle Wave (Onde triangulaire)");
                        ImGui::Spacing();
                        ImGui::TextWrapped("Plus douce que l'onde carrée. Contient des harmoniques impaires avec amplitude décroissante rapide.");
                        break;
                    case 5:
                        ImGui::TextWrapped(" Sawtooth (Dents de scie)");
                        ImGui::Spacing();
                        ImGui::TextWrapped("Contient toutes les harmoniques. Son brillant et riche, utilisé dans les synthétiseurs.");
                        break;
                    case 6:
                        ImGui::TextWrapped(" White Noise (Bruit blanc)");
                        ImGui::Spacing();
                        ImGui::TextWrapped("Contient toutes les fréquences avec la même intensité. Utilisé pour les tests audio et les effets sonores.");
                        break;
                    case 7:
                        ImGui::TextWrapped(" Pink Noise (Bruit rose)");
                        ImGui::Spacing();
                        ImGui::TextWrapped("Énergie égale par octave. Plus naturel que le bruit blanc, utilisé pour le calibrage audio.");
                        break;
                    default:
                        ImGui::TextWrapped("Sélectionnez un type de forme d'onde pour voir sa description.");
                        break;
                }
                
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                
                ImGui::SeparatorText(" Formule mathématique");
                ImGui::Spacing();
                
                switch (selectedWaveform) {
                    case 1:
                        ImGui::Text("y(t) = A × sin(2πft)");
                        break;
                    case 2:
                        ImGui::Text("y(t) = A × cos(2πft)");
                        break;
                    case 3:
                        ImGui::Text("y(t) = A × sgn(sin(2πft))");
                        break;
                    case 4:
                        ImGui::Text("y(t) = A × (2|sawtooth| - 1)");
                        break;
                    case 5:
                        ImGui::Text("y(t) = A × (2(t/T mod 1) - 1)");
                        break;
                    case 6:
                    case 7:
                        ImGui::Text("y(t) = A × random()");
                        break;
                }
                
                ImGui::Spacing();
                ImGui::Text("Où:");
                ImGui::BulletText("A = Amplitude (%.2f)", waveAmplitude);
                ImGui::BulletText("f = Fréquence (%.1f Hz)", waveFrequency);
                ImGui::BulletText("t = Temps");
                
                ImGui::EndChild();
                
                ImGui::EndTabItem();
            }
            
            ImGui::EndTabBar();
        }

        // Barre de statut en bas
        ImGui::Separator();
        ImGui::Text("📊 Statut: %s", statusMessage.c_str());

        ImGui::End();

        // Rendu
        ImGui::Render();
        SDL_SetRenderDrawColor(renderer, 25, 25, 30, 255);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    // Nettoyage
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}