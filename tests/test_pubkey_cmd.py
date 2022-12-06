from apps.neo_n3_cmd import Neo_n3_Command
from utils import create_simple_nav_instructions
from pathlib import Path
from ragger.navigator import NavInsID, NavIns
from ragger.bip import calculate_public_key_and_chaincode, CurveChoice

ROOT_SCREENSHOT_PATH = Path(__file__).parent.resolve()

def test_get_public_key_no_confirm(backend, firmware):
    client = Neo_n3_Command(backend)
    for path in ["m/44'/888'/0'/0/0", "m/44'/888'/1'/0/0", "m/44'/888'/10'/1/23"]:
        pub_key = client.get_public_key(bip44_path=path)
        ref_public_key, _ = calculate_public_key_and_chaincode(curve=CurveChoice.Nist256p1, path=path)
        assert pub_key.hex() == ref_public_key

from time import sleep
def test_get_public_key_confirm(backend, firmware, navigator, test_name):
    client = Neo_n3_Command(backend)
    path = "m/44'/888'/0'/0/0"
    with client.get_public_key_async(bip44_path=path):
        if backend.firmware.device == "nanos":
            nav_ins = create_simple_nav_instructions(3)
        elif backend.firmware.device.startswith("nano"):
            nav_ins = create_simple_nav_instructions(2)
        else:
            nav_ins = [
                       # NavIns(NavInsID.USE_CASE_ADDRESS_CONFIRMATION_SHOW_QR),
                       NavIns(NavInsID.TOUCH, (197,276)),
                       NavIns(NavInsID.USE_CASE_ADDRESS_CONFIRMATION_EXIT_QR),
                       NavIns(NavInsID.USE_CASE_ADDRESS_CONFIRMATION_CONFIRM)]
        navigator.navigate_and_compare(ROOT_SCREENSHOT_PATH, test_name, nav_ins, first_instruction_wait=2.0, middle_instruction_wait=1.0, last_instruction_wait=2.0)
    pub_key = backend.last_async_response.data

    ref_public_key, _ = calculate_public_key_and_chaincode(curve=CurveChoice.Nist256p1, path=path)
    assert pub_key.hex() == ref_public_key
