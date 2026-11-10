// Minimal MessagePack decoder for the plain msgpack produced by ArduinoJson's
// serializeMsgPack (and @msgpack/msgpack in the mock API). Maps decode to plain
// objects and 64-bit integers to Number. Extension types are not supported.

const decoder = new TextDecoder();

export const unpack = (input: ArrayBuffer | Uint8Array): unknown => {
  const bytes = input instanceof Uint8Array ? input : new Uint8Array(input);
  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  let pos = 0;

  // TextDecoder has a high per-call cost, so short ASCII strings (most keys) are built directly
  const str = (len: number): string => {
    if (len < 16) {
      let out = '';
      for (let i = pos; i < pos + len; i++) {
        const c = bytes[i]!;
        if (c > 0x7f) return decoder.decode(bytes.subarray(pos, (pos += len)));
        out += String.fromCharCode(c);
      }
      pos += len;
      return out;
    }
    return decoder.decode(bytes.subarray(pos, (pos += len)));
  };

  const bin = (len: number): Uint8Array => bytes.slice(pos, (pos += len));

  const arr = (len: number): unknown[] => {
    const out = new Array<unknown>(len);
    for (let i = 0; i < len; i++) out[i] = read();
    return out;
  };

  const map = (len: number): Record<string, unknown> => {
    const out: Record<string, unknown> = {};
    for (let i = 0; i < len; i++) {
      let key = String(read());
      if (key === '__proto__') key = '__proto_';
      out[key] = read();
    }
    return out;
  };

  // returns the current offset and advances past `size` bytes
  const skip = (size: number): number => (pos += size) - size;

  const read = (): unknown => {
    if (pos >= bytes.length) throw new Error('Unexpected end of MessagePack data');
    const t = bytes[pos++]!;

    if (t < 0x80) return t;
    if (t < 0x90) return map(t & 0x0f);
    if (t < 0xa0) return arr(t & 0x0f);
    if (t < 0xc0) return str(t & 0x1f);
    if (t >= 0xe0) return t - 0x100;

    switch (t) {
      case 0xc0:
        return null;
      case 0xc2:
        return false;
      case 0xc3:
        return true;
      case 0xc4:
        return bin(bytes[pos++]!);
      case 0xc5:
        return bin(view.getUint16(skip(2)));
      case 0xc6:
        return bin(view.getUint32(skip(4)));
      case 0xca:
        return view.getFloat32(skip(4));
      case 0xcb:
        return view.getFloat64(skip(8));
      case 0xcc:
        return bytes[pos++];
      case 0xcd:
        return view.getUint16(skip(2));
      case 0xce:
        return view.getUint32(skip(4));
      case 0xcf:
        return Number(view.getBigUint64(skip(8)));
      case 0xd0:
        return view.getInt8(skip(1));
      case 0xd1:
        return view.getInt16(skip(2));
      case 0xd2:
        return view.getInt32(skip(4));
      case 0xd3:
        return Number(view.getBigInt64(skip(8)));
      case 0xd9:
        return str(bytes[pos++]!);
      case 0xda:
        return str(view.getUint16(skip(2)));
      case 0xdb:
        return str(view.getUint32(skip(4)));
      case 0xdc:
        return arr(view.getUint16(skip(2)));
      case 0xdd:
        return arr(view.getUint32(skip(4)));
      case 0xde:
        return map(view.getUint16(skip(2)));
      case 0xdf:
        return map(view.getUint32(skip(4)));
      default:
        throw new Error(`Unsupported MessagePack type 0x${t.toString(16)}`);
    }
  };

  return read();
};
