type pixel = u32
type index = i32

let map2d 'a 'b [m] [n] (f: a -> b) (arr2d: [m][n]a): [m][n]b =
  map (map f) arr2d

let unpack_rgba (pixel: u32): (u8, u8, u8, u8) =
  (u8.u32 pixel,
   u8.u32 (pixel >> 8),
   u8.u32 (pixel >> 16),
   u8.u32 (pixel >> 24))

let pack_rgba (r: u8, g: u8, b: u8, a: u8): u32 =
  u32.u8 a << 24 | u32.u8 b << 16 | u32.u8 g << 8 | u32.u8 r

type rgba = (u8, u8, u8, u8)

let on_pixel_rgba (f: rgba -> rgba) (p: pixel): pixel =
  unpack_rgba p |> f |> pack_rgba

let channel_to_f64 (c: u8): f64 =
  f64.u8 c / 255.0

let channel_from_f64 (c: f64): u8 =
  u8.f64 (c * 255.0)
