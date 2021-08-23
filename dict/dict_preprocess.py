import struct
import zlib
import gzip
import xml.etree.ElementTree as ElementTree
import string
import unicodedata

apple_dict_body = 'AssetData/French - English.dictionary/Contents/Resources/Body.data'
mlex_filename = 'lefff-3.4.mlex/lefff-3.4.mlex'
output_filename = 'fren_dict.data'


class SyntaxInfo:
    def __init__(self, part_of_speech, lemma, morphosyntactic_tag):
        self.part_of_speech = part_of_speech
        self.lemma = lemma
        self.morphosyntactic_tag = morphosyntactic_tag


class DictEntry:
    def __init__(self, word):
        self.word = word
        self.syntax_infos = []
        self.definitions = []


class DictPreprocess:
    def __init__(self):
        self.dictionary = {}

    def get_dict_entry(self, word):
        dict_entry = self.dictionary.get(word)
        if dict_entry is None:
            dict_entry = DictEntry(word)
            self.dictionary[word] = dict_entry
        return dict_entry

    def read_mlex_file(self):
        print(f'read mlex file {mlex_filename}')
        with open(mlex_filename, 'r') as f:
            for line in f:
                word_raw, part_of_speech, lemma_raw, morphosyntactic_tag = line.rstrip('\n').split('\t')
                # part of speech:
                # https://web.archive.org/web/20200628045024/https://alpage.inria.fr/frmgwiki/content/tagset-frmg
                # morphosyntactic tag: see `lefff-tagset-0.1.2.pdf` that comes with lefff mlex data

                word_clean = DictPreprocess.clean_word(word_raw)
                if not DictPreprocess.word_allowed(word_clean):
                    continue

                lemma_clean = DictPreprocess.clean_word(lemma_raw)

                dict_entry = self.get_dict_entry(word_clean)
                dict_entry.syntax_infos.append(SyntaxInfo(part_of_speech, lemma_clean, morphosyntactic_tag))

    def read_apple_dict(self):
        # based off of https://gist.github.com/josephg/5e134adf70760ee7e49d
        print(f'read apple dictionary body from {apple_dict_body}')
        with open(apple_dict_body, 'rb') as f:
            f.seek(0x40)
            limit = 0x40 + struct.unpack('<i', f.read(4))[0]
            f.seek(0x60)
            while f.tell() < limit:
                sz, = struct.unpack('<i', f.read(4))
                buf = zlib.decompress(f.read(sz)[8:])

                pos = 0
                while pos < len(buf):
                    chunksize, = struct.unpack('<i', buf[pos:pos+4])
                    pos += 4

                    definition_xml = buf[pos:pos+chunksize].decode()
                    definition_root = ElementTree.fromstring(definition_xml)

                    definition_id = definition_root.attrib['id']
                    if definition_id.startswith('f_'):
                        # french
                        word_raw = definition_root.attrib['{http://www.apple.com/DTDs/DictionaryService-1.0.rng}title']
                        word_clean = DictPreprocess.clean_word(word_raw)
                        if DictPreprocess.word_allowed(word_clean):
                            dict_entry = self.get_dict_entry(word_clean)
                            dict_entry.definitions.append(definition_xml)
                        else:
                            print(f'warning, word in apple dictionary not allowed, {word_raw}')

                    elif definition_id.startswith('e_'):
                        # english to french definition
                        pass
                    else:
                        print(f'warning, unknown language for definition id {definition_id}')

                    pos += chunksize

    def write_output(self):
        print(f'write output to file {output_filename}')
        data_parts = [struct.pack('<I', len(self.dictionary))]
        for word, dict_entry in self.dictionary.items():
            data_parts.append(DictPreprocess.write_str(word))

            data_parts.append(struct.pack('B', len(dict_entry.syntax_infos)))
            for syntax_info in dict_entry.syntax_infos:
                data_parts.append(DictPreprocess.write_str(syntax_info.part_of_speech))
                if syntax_info.lemma == word:
                    data_parts.append(DictPreprocess.write_str(''))
                else:
                    data_parts.append(DictPreprocess.write_str(syntax_info.lemma))
                data_parts.append(DictPreprocess.write_str(syntax_info.morphosyntactic_tag))

            data_parts.append(struct.pack('B', len(dict_entry.definitions)))
            for definition in dict_entry.definitions:
                data_parts.append(DictPreprocess.write_str(definition))

        data_joined = b''.join(data_parts)

        data_compressed = gzip.compress(data_joined, zlib.Z_BEST_COMPRESSION, mtime=0)

        with open(output_filename, 'wb') as f:
            f.write(data_compressed)

    @staticmethod
    def clean_word(word):
        return word.lower().replace('œ', 'oe').replace('æ', 'ae')

    @staticmethod
    def char_is_accent(c):
        char_num = ord(c)
        return 0x300 <= char_num <= 0x36F

    @staticmethod
    def word_allowed(word_lowercase):
        word_accents_split = unicodedata.normalize('NFD', word_lowercase)
        word_no_accents = ''.join(filter(lambda c: not DictPreprocess.char_is_accent(c), word_accents_split))

        has_letter = False
        for i, c in enumerate(word_no_accents):
            if c in string.ascii_lowercase:
                has_letter = True
            elif c in ' \'-.' and i > 0:
                # allow some punctuation if after first character
                pass
            else:
                return False
        return has_letter  # do not allow words with only punctuation

    @staticmethod
    def write_str(my_str):
        my_str_bytes = my_str.encode('utf8')
        return struct.pack('<I', len(my_str_bytes)) + my_str_bytes

    @staticmethod
    def main():
        preprocessor = DictPreprocess()
        preprocessor.read_mlex_file()
        preprocessor.read_apple_dict()
        preprocessor.write_output()


if __name__ == '__main__':
    DictPreprocess.main()
