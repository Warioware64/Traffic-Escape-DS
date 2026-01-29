#!/usr/bin/env python3
"""
Traffic Escape DS - Level Editor
Creates level data files for the Nintendo DS game.

Usage:
    python level_editor.py

Controls:
    - Click on grid to place/select car
    - Use controls panel to set car properties
    - Save to .bin file for game loading
    - Export to C++ header for hardcoded levels
"""

import tkinter as tk
from tkinter import ttk, filedialog, messagebox
import struct
import os

# Car definitions matching the game
CAR_NAMES = ["Car", "Car2", "Car3", "Car4", "Car5", "Car5Police", "Car5Taxi", "Car8"]
CAR_SIZES = [2, 2, 2, 3, 2, 2, 2, 3]  # Number of cells each car occupies
CAR_COLORS = ["#FF6B6B", "#4ECDC4", "#45B7D1", "#96CEB4", "#FFEAA7", "#DDA0DD", "#F0E68C", "#87CEEB"]

TEXTURE_NAMES = [
    "car", "car_blue", "car_gray", "car_red",
    "car2", "car2_black", "car2_red",
    "car3", "car3_red", "car3_yellow",
    "car4", "car4_grey", "car4_lightgrey", "car4_lightorange",
    "car5", "car5_green", "car5_grey",
    "car5_police", "car5_police_la", "car5_taxi",
    "Car8", "Car8_grey", "Car8_mail", "Car8_purple"
]

ORIENTATIONS = [
    "0 - Horizontal (left anchor)",
    "1 - Horizontal (right anchor)",
    "2 - Vertical (top anchor)",
    "3 - Vertical (bottom anchor)"
]

# Background options (must match GameLevelLoader::BG_name_list)
BACKGROUND_NAMES = ["sky1", "sky2", "sky3", "sky4", "sky5", "sky6"]

# Orientation rules: 0,1 = LEFT_RIGHT, 2,3 = TOP_UP
ORIENTATION_HORIZONTAL = [0, 1]
ORIENTATION_VERTICAL = [2, 3]

GRID_SIZE = 6
MAX_CARS = 16
CELL_SIZE = 80


class Car:
    def __init__(self, car_id=0, orientation=0, texture=0, grid_x=0, grid_y=0):
        self.car_id = car_id
        self.orientation = orientation
        self.texture = texture
        self.grid_x = grid_x
        self.grid_y = grid_y
        self.warning_true = False

    def get_size(self):
        return CAR_SIZES[self.car_id]

    def is_horizontal(self):
        return self.orientation in ORIENTATION_HORIZONTAL

    def get_cells(self):
        """Returns list of (x, y) tuples for all cells this car occupies"""
        cells = [(self.grid_x, self.grid_y)]
        size = self.get_size()

        if self.is_horizontal():
            for i in range(1, size):
                cells.append((self.grid_x + i, self.grid_y))
        else:
            for i in range(1, size):
                cells.append((self.grid_x, self.grid_y + i))

        return cells

    def is_valid_position(self):
        """Check if car fits within grid bounds"""
        size = self.get_size()
        if self.is_horizontal():
            return self.grid_x + size <= GRID_SIZE and self.grid_y < GRID_SIZE
        else:
            return self.grid_x < GRID_SIZE and self.grid_y + size <= GRID_SIZE


class LevelEditor:
    def __init__(self, root):
        self.root = root
        self.root.title("Traffic Escape DS - Level Editor")
        self.root.geometry("1000x700")

        self.cars = []
        self.selected_car_index = None
        self.background_id = 0  # Default background

        self.setup_ui()
        self.draw_grid()

    def setup_ui(self):
        # Main container
        main_frame = ttk.Frame(self.root, padding="10")
        main_frame.grid(row=0, column=0, sticky="nsew")

        # Left side - Grid canvas
        grid_frame = ttk.LabelFrame(main_frame, text="Level Grid (6x6)", padding="5")
        grid_frame.grid(row=0, column=0, rowspan=2, sticky="nsew", padx=(0, 10))

        self.canvas = tk.Canvas(
            grid_frame,
            width=CELL_SIZE * GRID_SIZE,
            height=CELL_SIZE * GRID_SIZE,
            bg="white"
        )
        self.canvas.pack()
        self.canvas.bind("<Button-1>", self.on_grid_click)

        # Right side - Controls
        controls_frame = ttk.LabelFrame(main_frame, text="Car Properties", padding="10")
        controls_frame.grid(row=0, column=1, sticky="new")

        # Car Type
        ttk.Label(controls_frame, text="Car Type:").grid(row=0, column=0, sticky="w", pady=2)
        self.car_type_var = tk.StringVar(value=CAR_NAMES[0])
        car_type_combo = ttk.Combobox(controls_frame, textvariable=self.car_type_var, values=CAR_NAMES, width=20)
        car_type_combo.grid(row=0, column=1, pady=2)
        car_type_combo.bind("<<ComboboxSelected>>", self.on_property_change)

        # Car size info
        self.size_label = ttk.Label(controls_frame, text="Size: 2 cells")
        self.size_label.grid(row=1, column=1, sticky="w", pady=2)

        # Orientation
        ttk.Label(controls_frame, text="Orientation:").grid(row=2, column=0, sticky="w", pady=2)
        self.orientation_var = tk.StringVar(value=ORIENTATIONS[0])
        orientation_combo = ttk.Combobox(controls_frame, textvariable=self.orientation_var, values=ORIENTATIONS, width=25)
        orientation_combo.grid(row=2, column=1, pady=2)
        orientation_combo.bind("<<ComboboxSelected>>", self.on_property_change)

        # Texture
        ttk.Label(controls_frame, text="Texture:").grid(row=3, column=0, sticky="w", pady=2)
        self.texture_var = tk.StringVar(value=TEXTURE_NAMES[0])
        texture_combo = ttk.Combobox(controls_frame, textvariable=self.texture_var, values=TEXTURE_NAMES, width=20)
        texture_combo.grid(row=3, column=1, pady=2)
        texture_combo.bind("<<ComboboxSelected>>", self.on_property_change)

        # Grid position
        pos_frame = ttk.Frame(controls_frame)
        pos_frame.grid(row=4, column=0, columnspan=2, pady=5)

        ttk.Label(pos_frame, text="Grid X:").pack(side="left")
        self.grid_x_var = tk.IntVar(value=0)
        self.grid_x_spin = ttk.Spinbox(pos_frame, from_=0, to=5, width=5, textvariable=self.grid_x_var, command=self.on_property_change)
        self.grid_x_spin.pack(side="left", padx=5)

        ttk.Label(pos_frame, text="Grid Y:").pack(side="left")
        self.grid_y_var = tk.IntVar(value=0)
        self.grid_y_spin = ttk.Spinbox(pos_frame, from_=0, to=5, width=5, textvariable=self.grid_y_var, command=self.on_property_change)
        self.grid_y_spin.pack(side="left", padx=5)

        # Buttons
        btn_frame = ttk.Frame(controls_frame)
        btn_frame.grid(row=5, column=0, columnspan=2, pady=10)

        ttk.Button(btn_frame, text="Add Car", command=self.add_car).pack(side="left", padx=2)
        ttk.Button(btn_frame, text="Update Car", command=self.update_car).pack(side="left", padx=2)
        ttk.Button(btn_frame, text="Delete Car", command=self.delete_car).pack(side="left", padx=2)

        # Car list
        list_frame = ttk.LabelFrame(main_frame, text=f"Cars (0/{MAX_CARS})", padding="5")
        list_frame.grid(row=1, column=1, sticky="nsew", pady=(10, 0))
        self.list_frame_label = list_frame

        self.car_listbox = tk.Listbox(list_frame, width=35, height=10)
        self.car_listbox.pack(fill="both", expand=True)
        self.car_listbox.bind("<<ListboxSelect>>", self.on_list_select)

        # Level Properties (background)
        level_frame = ttk.LabelFrame(main_frame, text="Level Properties", padding="10")
        level_frame.grid(row=2, column=0, columnspan=2, sticky="ew", pady=(10, 0))

        ttk.Label(level_frame, text="Background:").pack(side="left", padx=5)
        self.background_var = tk.StringVar(value=BACKGROUND_NAMES[0])
        bg_combo = ttk.Combobox(level_frame, textvariable=self.background_var, values=BACKGROUND_NAMES, width=15)
        bg_combo.pack(side="left", padx=5)
        bg_combo.bind("<<ComboboxSelected>>", self.on_background_change)

        # File operations
        file_frame = ttk.LabelFrame(main_frame, text="File Operations", padding="10")
        file_frame.grid(row=3, column=0, columnspan=2, sticky="ew", pady=(10, 0))

        ttk.Button(file_frame, text="Save Binary (.bin)", command=self.save_binary).pack(side="left", padx=5)
        ttk.Button(file_frame, text="Load Binary (.bin)", command=self.load_binary).pack(side="left", padx=5)
        ttk.Button(file_frame, text="Export C++ Header", command=self.export_cpp).pack(side="left", padx=5)
        ttk.Button(file_frame, text="Clear All", command=self.clear_all).pack(side="left", padx=5)

        # Status bar
        self.status_var = tk.StringVar(value="Ready. Click on grid to place cars.")
        status_bar = ttk.Label(main_frame, textvariable=self.status_var, relief="sunken")
        status_bar.grid(row=4, column=0, columnspan=2, sticky="ew", pady=(10, 0))

    def draw_grid(self):
        """Draw the 6x6 grid and all cars"""
        self.canvas.delete("all")

        # Draw grid lines
        for i in range(GRID_SIZE + 1):
            # Vertical lines
            x = i * CELL_SIZE
            self.canvas.create_line(x, 0, x, GRID_SIZE * CELL_SIZE, fill="gray")
            # Horizontal lines
            y = i * CELL_SIZE
            self.canvas.create_line(0, y, GRID_SIZE * CELL_SIZE, y, fill="gray")

        # Draw grid coordinates
        for x in range(GRID_SIZE):
            for y in range(GRID_SIZE):
                cx = x * CELL_SIZE + 5
                cy = y * CELL_SIZE + 5
                self.canvas.create_text(cx, cy, text=f"({x},{y})", anchor="nw", fill="lightgray", font=("Arial", 8))

        # Draw cars
        for idx, car in enumerate(self.cars):
            self.draw_car(car, idx == self.selected_car_index)

    def draw_car(self, car, selected=False):
        """Draw a single car on the grid"""
        cells = car.get_cells()
        color = CAR_COLORS[car.car_id % len(CAR_COLORS)]
        outline_color = "red" if selected else "black"
        outline_width = 3 if selected else 1

        # Calculate bounding box
        min_x = min(c[0] for c in cells)
        max_x = max(c[0] for c in cells)
        min_y = min(c[1] for c in cells)
        max_y = max(c[1] for c in cells)

        x1 = min_x * CELL_SIZE + 5
        y1 = min_y * CELL_SIZE + 5
        x2 = (max_x + 1) * CELL_SIZE - 5
        y2 = (max_y + 1) * CELL_SIZE - 5

        # Draw rounded rectangle for car
        self.canvas.create_rectangle(x1, y1, x2, y2, fill=color, outline=outline_color, width=outline_width)

        # Draw car info
        cx = (x1 + x2) / 2
        cy = (y1 + y2) / 2
        text = f"{CAR_NAMES[car.car_id]}\nO:{car.orientation}"
        self.canvas.create_text(cx, cy, text=text, font=("Arial", 9, "bold"), fill="black")

    def on_grid_click(self, event):
        """Handle click on grid"""
        grid_x = event.x // CELL_SIZE
        grid_y = event.y // CELL_SIZE

        if 0 <= grid_x < GRID_SIZE and 0 <= grid_y < GRID_SIZE:
            # Check if clicking on existing car
            for idx, car in enumerate(self.cars):
                if (grid_x, grid_y) in car.get_cells():
                    self.select_car(idx)
                    return

            # Update position spinboxes for new car placement
            self.grid_x_var.set(grid_x)
            self.grid_y_var.set(grid_y)
            self.selected_car_index = None
            self.draw_grid()
            self.status_var.set(f"Position ({grid_x}, {grid_y}) selected. Click 'Add Car' to place.")

    def select_car(self, idx):
        """Select a car and update UI"""
        self.selected_car_index = idx
        car = self.cars[idx]

        self.car_type_var.set(CAR_NAMES[car.car_id])
        self.orientation_var.set(ORIENTATIONS[car.orientation])
        self.texture_var.set(TEXTURE_NAMES[car.texture])
        self.grid_x_var.set(car.grid_x)
        self.grid_y_var.set(car.grid_y)

        self.update_size_label()
        self.car_listbox.selection_clear(0, tk.END)
        self.car_listbox.selection_set(idx)
        self.draw_grid()
        self.status_var.set(f"Selected car {idx}: {CAR_NAMES[car.car_id]} at ({car.grid_x}, {car.grid_y})")

    def on_list_select(self, event):
        """Handle selection in car list"""
        selection = self.car_listbox.curselection()
        if selection:
            self.select_car(selection[0])

    def on_property_change(self, event=None):
        """Handle property change in UI"""
        car_id = CAR_NAMES.index(self.car_type_var.get())
        self.update_size_label()

        # Update selected car if any
        if self.selected_car_index is not None:
            self.update_car()

    def on_background_change(self, event=None):
        """Handle background selection change"""
        self.background_id = BACKGROUND_NAMES.index(self.background_var.get())
        self.status_var.set(f"Background set to: {self.background_var.get()}")

    def update_size_label(self):
        """Update the car size label"""
        car_id = CAR_NAMES.index(self.car_type_var.get())
        size = CAR_SIZES[car_id]
        self.size_label.config(text=f"Size: {size} cells")

    def get_car_from_ui(self):
        """Create a Car object from current UI values"""
        return Car(
            car_id=CAR_NAMES.index(self.car_type_var.get()),
            orientation=ORIENTATIONS.index(self.orientation_var.get()),
            texture=TEXTURE_NAMES.index(self.texture_var.get()),
            grid_x=self.grid_x_var.get(),
            grid_y=self.grid_y_var.get()
        )

    def check_collision(self, new_car, exclude_idx=None):
        """Check if new car collides with existing cars"""
        new_cells = set(new_car.get_cells())

        for idx, car in enumerate(self.cars):
            if idx == exclude_idx:
                continue
            if new_cells & set(car.get_cells()):
                return True
        return False

    def add_car(self):
        """Add a new car to the level"""
        if len(self.cars) >= MAX_CARS:
            messagebox.showwarning("Limit Reached", f"Maximum {MAX_CARS} cars per level.")
            return

        car = self.get_car_from_ui()

        #if not car.is_valid_position():
            #messagebox.showwarning("Invalid Position", "Car doesn't fit within grid bounds.")
            #return

        #if self.check_collision(car):
            #messagebox.showwarning("Collision", "Car overlaps with existing car.")
            #return

        self.cars.append(car)
        self.selected_car_index = len(self.cars) - 1
        self.update_car_list()
        self.draw_grid()
        self.status_var.set(f"Added {CAR_NAMES[car.car_id]} at ({car.grid_x}, {car.grid_y})")

    def update_car(self):
        """Update the selected car"""
        if self.selected_car_index is None:
            return

        car = self.get_car_from_ui()


        #if not car.is_valid_position() and car.warning_true == False:
            #messagebox.showwarning("Invalid Position", "Car doesn't fit within grid bounds.")
            #car.warning_true = True
            #return
        
        #if self.check_collision(car, self.selected_car_index) and car.warning_true == False:
            #messagebox.showwarning("Collision", "Car overlaps with existing car.")
            #car.warning_true = True
            #return

        self.cars[self.selected_car_index] = car
        self.update_car_list()
        self.draw_grid()
        self.status_var.set(f"Updated car {self.selected_car_index}")

    def delete_car(self):
        """Delete the selected car"""
        if self.selected_car_index is None:
            messagebox.showinfo("No Selection", "Select a car to delete.")
            return

        del self.cars[self.selected_car_index]
        self.selected_car_index = None
        self.update_car_list()
        self.draw_grid()
        self.status_var.set("Car deleted.")

    def update_car_list(self):
        """Update the car listbox"""
        self.car_listbox.delete(0, tk.END)
        for idx, car in enumerate(self.cars):
            text = f"{idx}: {CAR_NAMES[car.car_id]} @ ({car.grid_x},{car.grid_y}) O:{car.orientation}"
            self.car_listbox.insert(tk.END, text)

        self.list_frame_label.config(text=f"Cars ({len(self.cars)}/{MAX_CARS})")

    def clear_all(self):
        """Clear all cars and reset background"""
        if self.cars and not messagebox.askyesno("Confirm", "Clear all cars?"):
            return

        self.cars = []
        self.selected_car_index = None
        self.background_id = 0
        self.background_var.set(BACKGROUND_NAMES[0])
        self.update_car_list()
        self.draw_grid()
        self.status_var.set("All cars cleared.")

    def save_binary(self):
        """Save level to binary file"""
        filename = filedialog.asksaveasfilename(
            defaultextension=".bin",
            filetypes=[("Binary files", "*.bin"), ("All files", "*.*")],
            title="Save Level Binary"
        )

        if not filename:
            return

        try:
            with open(filename, 'wb') as f:
                # Write header: number of cars, background ID
                f.write(struct.pack('BB', len(self.cars), self.background_id))

                # Write each car: true_car, carID, orientation, tex, grid_x, grid_y
                for car in self.cars:
                    f.write(struct.pack('BBBBBB',
                        1,  # true_car
                        car.car_id,
                        car.orientation,
                        car.texture,
                        car.grid_x,
                        car.grid_y
                    ))

                # Pad to MAX_CARS entries
                for _ in range(MAX_CARS - len(self.cars)):
                    f.write(struct.pack('BBBBBB', 0, 0, 0, 0, 0, 0))

            self.status_var.set(f"Saved to {os.path.basename(filename)}")
            messagebox.showinfo("Saved", f"Level saved to {filename}")

        except Exception as e:
            messagebox.showerror("Error", f"Failed to save: {e}")

    def load_binary(self):
        """Load level from binary file"""
        filename = filedialog.askopenfilename(
            filetypes=[("Binary files", "*.bin"), ("All files", "*.*")],
            title="Load Level Binary"
        )

        if not filename:
            return

        try:
            with open(filename, 'rb') as f:
                # Read header: number of cars, background ID
                header = struct.unpack('BB', f.read(2))
                num_cars = header[0]
                self.background_id = header[1]

                # Update background combobox
                if self.background_id < len(BACKGROUND_NAMES):
                    self.background_var.set(BACKGROUND_NAMES[self.background_id])

                self.cars = []
                for _ in range(MAX_CARS):
                    data = struct.unpack('BBBBBB', f.read(6))
                    true_car, car_id, orientation, texture, grid_x, grid_y = data

                    if true_car == 1:
                        self.cars.append(Car(car_id, orientation, texture, grid_x, grid_y))

            self.selected_car_index = None
            self.update_car_list()
            self.draw_grid()
            self.status_var.set(f"Loaded {len(self.cars)} cars, BG: {BACKGROUND_NAMES[self.background_id]} from {os.path.basename(filename)}")

        except Exception as e:
            messagebox.showerror("Error", f"Failed to load: {e}")

    def export_cpp(self):
        """Export level as C++ code"""
        filename = filedialog.asksaveasfilename(
            defaultextension=".hpp",
            filetypes=[("C++ Header", "*.hpp"), ("All files", "*.*")],
            title="Export C++ Header"
        )

        if not filename:
            return

        try:
            with open(filename, 'w') as f:
                f.write("// Auto-generated level data\n")
                f.write("#pragma once\n")
                f.write('#include "PosVehicules.hpp"\n\n')

                f.write("constexpr LevelData generated_level = {{\n")

                for car in self.cars:
                    f.write(f"    {{.true_car = 1, .ptrMesh = nullptr, .texGLptr = 0, ")
                    f.write(f".carID = {car.car_id}, .orientation = {car.orientation}, ")
                    f.write(f".tex = {car.texture}, .basepose = {{0,0,0}}, ")
                    f.write(f".grid2d = {{{car.grid_x}, {car.grid_y}}}}},\n")

                # Fill remaining slots with empty cars
                for _ in range(MAX_CARS - len(self.cars)):
                    f.write("    {.true_car = 0, .ptrMesh = nullptr, .texGLptr = 0, ")
                    f.write(".carID = 0, .orientation = 0, .tex = 0, .basepose = {0,0,0}, ")
                    f.write(".grid2d = {0, 0}},\n")

                f.write("}};\n")

            self.status_var.set(f"Exported to {os.path.basename(filename)}")
            messagebox.showinfo("Exported", f"C++ header saved to {filename}")

        except Exception as e:
            messagebox.showerror("Error", f"Failed to export: {e}")


def main():
    root = tk.Tk()
    app = LevelEditor(root)
    root.mainloop()


if __name__ == "__main__":
    main()
