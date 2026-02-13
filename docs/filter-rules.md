# Filter Rule String Reference

Complete guide to CGE filter effect rule strings. Use the [cge-tools editor](https://github.com/wysaid/cge-tools) to generate complex filter configurations visually.

## Rule String Syntax

Each filter step starts with `@` followed by the filter name and parameters. Multiple filters can be chained:

```
@method1 <args1> @method2 <args2> @method3 <args3>
```

Whitespace between `@` and method name is optional: both `@method` and `@ method` are valid.

### Special Directives

#### `#unpack` (Advanced)

Add `#unpack` at the start to bypass the `MultipleEffects` wrapper and add filters directly to the handler:

```
#unpack @adjust brightness 0.5
```

## Filter Methods

### 1. curve — Color Curve Adjustment

Apply color curves to RGB channels individually or combined.

**Format**: `@curve <channel> <points> [<channel> <points>]...`

**Channels**:
- `RGB` — Apply to all channels
- `R` / `G` / `B` — Individual red, green, blue channels

**Points**: `(x1, y1) (x2, y2) ... (xn, yn)`
- Range: `[0, 255]` for both x and y
- Minimum 2 points required
- Parentheses and coordinate separators (space, comma) are flexible

**Examples**:

```
①  RGB curve with 3 control points:
@curve RGB (0, 0) (100, 200) (255, 255)

② Multiple channel curves:
@curve R (0, 0) (50, 25) (255, 255) G (0, 0) (100, 150) (255, 255) RGB (0, 0) (200, 150) (255, 255)

③ Merging consecutive curves (more efficient):
@curve RGB (0, 0) (100, 200) (255, 255) R (0, 0) (50, 25) (255, 255) G (0, 0) (100, 150) (255, 255)
```

**Performance Tip**: When applying multiple curves consecutively, merge them into one `@curve` command to reduce overhead.

---

### 2. blend — Texture Blending

Blend an image texture with various blend modes. The texture is scaled to fit the destination image.

#### 2.1 Normal Blend

**Format**: `@blend <mode> <texture> <intensity>`

**Blend Modes** (with abbreviations in brackets):

| Category | Modes |
|----------|-------|
| **Basic** | `mix`, `dissolve` `[dsv]` |
| **Darken** | `darken` `[dk]`, `multiply` `[mp]`, `colorburn` `[cb]`, `linearburn` `[lb]`, `darkercolor` `[dc]` |
| **Lighten** | `lighten` `[lt]`, `screen` `[sr]`, `colordodge` `[cd]`, `lineardodge` `[ld]`, `lightercolor` `[lc]` |
| **Contrast** | `overlay` `[ol]`, `softlight` `[sl]`, `hardlight` `[hl]`, `vividlight` `[vvl]`, `linearlight` `[ll]`, `pinlight` `[pl]`, `hardmix` `[hm]` |
| **Difference** | `difference` `[dif]`, `exclude` `[ec]`, `subtract` `[sub]`, `divide` `[div]` |
| **Component** | `hue`, `saturation` `[sat]`, `color` `[cl]`, `luminosity` `[lum]` |
| **Special** | `add`, `addrev`, `colorbw` |

See [Photoshop Blend Modes](https://en.wikipedia.org/wiki/Blend_modes) for detailed mode descriptions.

**Parameters**:
- `<texture>`: Image filename with extension (e.g., `"image.jpg"`)
  - **Advanced**: Use `[texID,width,height]` for direct texture ID (e.g., `[10,1024,768]`). ⚠️ Use with caution — incorrect ID can break the app.
- `<intensity>`: Blend strength, range `(0, 100]`

**Example**:

```
80% overlay blend with "src.jpg":
@blend overlay src.jpg 80
@blend ol src.jpg 80  // Abbreviation works too
```

#### 2.2 krblend — Aspect-Ratio Preserving Blend

Same as `blend` but maintains the texture's aspect ratio (like AspectFill).

**Format**: `@krblend <mode> <texture> <intensity>`

#### 2.3 pixblend — Solid Color Blend

Blend with a solid color instead of a texture.

**Format**: `@pixblend <mode> <R> <G> <B> <A> <intensity>`

**Color Parameters**: 
- Range: `[0, 1]` or `[0, 255]`
- If alpha > 1.0, all values are interpreted as `[0, 255]`

**Example**:

```
90% overlay with pure red:
@pixblend overlay 1 0 0 1 90
@pixblend overlay 255 0 0 255 90  // Equivalent
```

#### 2.4 selfblend — Self Blend

Blend the image with itself (no texture parameter).

**Format**: `@selfblend <mode> <intensity>`

#### 2.5 vigblend — Vignette Blend

Blend with color using a vignette mask.

**Format**: `@vigblend <mode> <R> <G> <B> <A> <intensity> <low> <range> <centerX> <centerY> [kind]`

**Vignette Parameters**:
- `<low>`, `<range>`: Control vignette falloff
- `<centerX>`, `<centerY>`: Vignette center (typically `0.5` for image center)
- `[kind]`: Optional vignette mode (`0`-`3`)
  - `0`: Linear, no alpha (default)
  - `1`: Linear with alpha blending
  - `2`: Bilinear, no alpha
  - `3`: Bilinear with alpha

**Example**:

```
@vigblend overlay 0 0 0 1 50 0.02 0.45 0.5 0.5 0
@vigblend mix 10 10 30 255 100 0 1.5 0.5 0.5 1
```

---

### 3. adjust — Image Adjustments

Apply common image adjustments.

**Format**: `@adjust <function> <args...>`

| Function | Parameters | Range | Description |
|----------|------------|-------|-------------|
| **brightness** | `intensity` | `[-1, 1]` | Brightness adjustment |
| **contrast** | `intensity` | `> 0` | `< 1`: lower, `= 1`: none, `> 1`: higher |
| **saturation** | `intensity` | `≥ 0` | `0`: monochrome, `1`: none, `> 1`: higher |
| **monochrome** | `red green blue cyan magenta yellow` | `[-2, 3]` | Black & white adjustment (Photoshop-style) |
| **sharpen** | `intensity` | `[0, 10]` | Sharpness (`0` = no effect) |
| **blur** | `intensity` | `[0, 10]` | Blur (`0` = no effect, not recommended — use `@blur` instead) |
| **whitebalance** | `temperature tint` | `[-1, 1]`, `[0, 5]` | `temperature` `0` = none, `tint` `1` = none |
| **shadowhighlight** `[shl]` | `shadow highlight` | `[-200, 100]`, `[-100, 200]` | `0` = no effect |
| **hsv** | `red green blue magenta yellow cyan` | `[-1, 1]` | HSV adjustment per hue |
| **hsl** | `hue saturation luminance` | `[-1, 1]` | HSL adjustment |
| **level** | `dark light gamma` | `[0, 1]`, `dark < light` | Levels adjustment |
| **exposure** | `intensity` | `[-2, 2]` | Exposure adjustment |
| **colorbalance** | `redShift greenShift blueShift` | `[-1, 1]` | Color balance |
| **lut** | `filename` | — | Lookup table (512×512 PNG) |

**Examples**:

```
@adjust brightness 0.5
@adjust hsl -0.66 0.34 0.15
@adjust lut lookup.png  // Requires 512×512 LUT image
```

---

### 4. vignette — Vignette Effect

Add a radial vignette to darken image edges.

**Format**: `@vignette <low> <range> [<centerX> <centerY>]`

**Parameters**:
- `<low>`, `<range>`: Control vignette intensity and falloff
- `<centerX>`, `<centerY>`: Optional center (default `0.5 0.5`)

**Examples**:

```
@vignette 0.1 0.9
@vignette 0.1 0.9 0.5 0.5
```

---

### 5. colormul — Color Multiplication

Directly multiply pixel color values.

**Format**: `@colormul <function> <args...>`

#### Functions:

**① `flt` — Scalar Multiplication**

Multiply all RGB channels by a scalar.

```
@colormul flt 1.5  // Brighten by 1.5×
Result: dst.rgb = src.rgb * 1.5
```

**② `vec` — Vector Multiplication**

Multiply each channel independently.

```
@colormul vec 0.2 1.5 0.3
Result: dst.rgb = src.rgb * vec3(0.2, 1.5, 0.3)

// Pure red filter (faster than curve):
@colormul vec 1 0 0
```

**③ `mat` — Matrix Multiplication**

Multiply by a 3×3 color matrix (9 parameters, column-major).

```
// Monochrome (standard luminance weights):
@colormul mat 0.299 0.587 0.114  0.299 0.587 0.114  0.299 0.587 0.114

// Swap red and green channels:
@colormul mat 0 1 0  1 0 0  0 0 1
```

---

### 6. selcolor — Selective Color

Adjust specific color ranges (like Photoshop's Selective Color).

**Format**: `@selcolor <color1> (<c> <m> <y> <k>) [<color2> (<c> <m> <y> <k>)]...`

**Colors**: `red`, `green`, `blue`, `cyan`, `magenta`, `yellow`, `white`, `gray`, `black`

**CMYK Shifts**: Range `[-1, 1]` for cyan, magenta, yellow, black adjustments

**Example**:

```
@selcolor red (0, -0.29, 0, 0) green (0, 0, 0.07, 0) blue (0, 0.39, 0, 0) yellow (-0.39, 0.39, 0.39, -0.39) white (0, 0, 0.24, 0)
```

---

### 7. style — Artistic Styles

Apply artistic image effects.

**Format**: `@style <function> <args...>`

| Function | Parameters | Description |
|----------|------------|-------------|
| **crosshatch** | `spacing` `[0, 0.1]`, `lineWidth` `(0, 0.01]` | Cross-hatching pattern |
| **edge** | `mix` `[0, 1]`, `stride` `(0, 5]` | Edge detection (Sobel) |
| **emboss** | `mix` `[0, 1]`, `stride` `[1, 5]`, `angle` `[0, 2π]` | Emboss effect |
| **halftone** | `dotSize` `≥ 1` | Halftone dot pattern |
| **haze** | `distance` `[-0.5, 0.5]`, `slope` `[-0.5, 0.5]`, `r g b` `[0, 1]` | Haze/fog effect |
| **polkadot** | `dotScaling` `(0, 1]` | Polka dot pattern |
| **sketch** | `intensity` `[0, 1]` | Pencil sketch effect |
| **max** | — | Maximum filter |
| **min** | — | Minimum filter |
| **mid** | — | Median filter |

**Examples**:

```
@style crosshatch 0.03 0.003
@style edge 0.8 2.0
@style emboss 1.0 1.5 1.57
@style sketch 0.7
```

---

### 8. beautify — Beauty Filters

Skin smoothing and beautification effects.

**Format**: `@beautify <function> <args...>`

| Function | Parameters | Description |
|----------|------------|-------------|
| **bilateral** | `blurScale` `[-100, 100]`, `distanceFactor` `[1, 20]`, `[repeat]` `≥ 1` | Bilateral filter (edge-preserving blur) |
| **face** | `intensity` `(0, 1]`, `[width]`, `[height]` | Face beautification (defaults: 720×1280) |

**Examples**:

```
@beautify bilateral 5 10 2  // radius=5, tolerance=10, repeat 2 times
@beautify face 1.0 720 1280
@beautify face 0.8  // Auto dimensions
```

---

### 9. blur — Advanced Blur

Specialized blur algorithms (better than `@adjust blur`).

**Format**: `@blur <function> <args...>`

| Function | Parameters | Description |
|----------|------------|-------------|
| **lerp** | `level` `[0, 1]`, `base` `[0.6, 2.0]` | Lerp blur algorithm |

**Example**:

```
@blur lerp 0.7 1.2
```

---

### 10. dynamic — Animated Effects

Dynamic filters with motion over time (not supported in static editors).

**Format**: `@dynamic <function> <args...>`

| Function | Parameters | Description |
|----------|------------|-------------|
| **wave** | `autoMotionSpeed` | Auto-animated wave ripple |
|  | `motion angle strength [autoMotionSpeed]` | Manual wave parameters |
| **motionFlow** `[mf]` | `totalFrames frameDelay` | Motion trail effect |

**Examples**:

```
@dynamic wave 0.5  // Auto motion with speed 0.5
@dynamic wave 0.0 1.57 5.0  // Static wave: motion=0, angle=π/2, strength=5
@dynamic wave 0.0 1.57 5.0 0.3  // Animated with custom params
@dynamic motionFlow 10 2  // 10 frames with 2-frame delay
```

---

### 11. lomo — Lomo Effect

Lomography-style effect combining vignette and color grading.

**Format**: `@lomo <vignetteStart> <vignetteEnd> <colorScaleLow> <colorScaleRange> <saturation> <isLinear>`

**Parameters**:
- `<vignetteStart>`, `<vignetteEnd>`: Vignette parameters (typically `< 1`)
- `<colorScaleLow>`, `<colorScaleRange>`: Color scale adjustments (typically `< 1`)
- `<saturation>`: Saturation multiplier `[0, 1]`
- `<isLinear>`: `0` or `1` for vignette type

**Example**:

```
@lomo 0.2 0.8 0.1 0.2 1.0 0
```

---

### 12. cvlomo — Lomo with Curve

Lomo effect with custom curve adjustment.

**Format**: `@cvlomo <vignetteStart> <vignetteEnd> <colorScaleLow> <colorScaleRange> <saturation> <curve_string>`

**Note**: `<curve_string>` is a curve definition **without** the `@curve` prefix.

**Example**:

```
@cvlomo 0.2 0.8 0.1 0.2 1.0 RGB (0, 0) (255, 255)
```

---

### 13. colorscale — Color Scaling

Apply color scaling adjustments (CPU-intensive, may affect performance).

**Format**: `@colorscale <low> <range> <saturation>`

---

### 14. special — Custom Effects

Hardcoded special effects by index.

**Format**: `@special <N>`

`<N>`: Effect index number (implementation-specific)

---

## Complete Example

Combining multiple filters:

```
@curve RGB (0, 0) (128, 150) (255, 255) 
@adjust hsl 0.02 -0.31 -0.17 
@blend overlay texture.jpg 80 
@vignette 0.1 0.9 
@beautify face 0.8
```

## Notes

- **Spaces**: Flexible between parameters and parentheses
- **Abbreviations**: Use short forms for faster typing (e.g., `ol` for `overlay`)
- **Case**: Channel names are case-insensitive (`R` = `r`, `RGB` = `rgb`)
- **Performance**: Merge consecutive `@curve` commands; prefer `@colormul vec` over curves for simple channel operations
- **Texture Loading**: Requires custom image-loading callback (see demo for implementation)
- **Tool**: Use [cge-tools](https://github.com/wysaid/cge-tools) for visual filter editing

## Version History

- **v0.2.1.x**: Methods 1-12
- **v0.3.2.1**: Added `vigblend`
- **2014-11-12**: Added `selcolor`
- **2015-02-05**: Added `style` methods
- **2015-03-19**: Added `beautify` methods
- **2015-08-07**: Added `#unpack` directive
- **2015-08-08**: Added `@blur` methods
- **2015-11-18**: Added `@dynamic` methods
