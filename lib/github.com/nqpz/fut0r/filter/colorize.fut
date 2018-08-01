-- | Colorize an image.
--
-- Set the hue, saturation, and lightness of an image based on the luminosity of
-- the pixels.
--
-- Author: Niels G. W. Serup
--
-- Ported from frei0r's filter/colorize.c.  Copyright (C) 2012 Janne Liljeblad.
-- In turn ported from Gimp's gimpoperationcolorize.c, gimpcolortypes.h, and
-- gimpcolorspace.c.  Copyright (C) 1995 Spencer Kimball and Peter Mattis.
-- Copyright (C) 2007 Michael Natterer.

import "/futlib/functional"
import "../base"

let hsl_value (n1: f64) (n2: f64) (hue: f64): f64 =
  let hue' = if hue > 6.0
             then hue - 6.0
             else if hue < 0.0
             then hue + 6.0
             else hue
  in if hue' < 1.0
     then n1 + (n2 - n1) * hue'
     else if hue' < 3.0
     then n2
     else if hue' < 4.0
     then n1 + (n2 - n1) * (4.0 - hue')
     else n1

let hsl_to_rgb (h: f64, s: f64, l: f64): (f64, f64, f64) =
  if s == 0.0
  then (l, l, l)
  else let m2 = if l <= 0.5
                then l * (1.0 + s)
                else l + s - l * s
       let m1 = 2.0 * l - m2
       let r = hsl_value m1 m2 (h * 6.0 + 2.0)
       let g = hsl_value m1 m2 (h * 6.0)
       let b = hsl_value m1 m2 (h * 6.0 - 2.0)
       in (r, g, b)

let luminance (r: f64) (g: f64) (b: f64): f64 =
  let lum_r = 0.2126
  let lum_g = 0.7152
  let lum_b = 0.0722
  in r * lum_r + g * lum_g + b * lum_b

-- | Colorize an image.
let colorize [height] [width]
      (hue: f64) (saturation: f64) (lightness: f64)
      (image: [height][width]pixel): [height][width]pixel =
  let lightness' = lightness - 0.5
  let transform (r, g, b, a) =
    let lum = luminance (channel_to_f64 r) (channel_to_f64 g) (channel_to_f64 b)
    let lum' = if lightness' > 0.0
               then lum * (1.0 - lightness') + 1.0 - (1.0 - lightness')
               else if lightness' < 0.0
               then lum * (lightness' + 1.0)
               else lum
    let (r', g', b') = hsl_to_rgb (hue, saturation, lum')
    in (channel_from_f64 r', channel_from_f64 g', channel_from_f64 b', a)
  in map2d (on_pixel_rgba transform) image

entry f0r_update [height] [width]
      (hue: f64) (saturation: f64) (lightness: f64)
      (_time: f64) (image: [height][width]pixel): [height][width]pixel =
  colorize hue saturation lightness image
